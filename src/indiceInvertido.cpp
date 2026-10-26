#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <semaphore.h>
#include "../include/indiceInvertido.h"

using namespace std;
namespace fs = std::filesystem;

static bool validarEnteroPositivo(const string &str, int &valorSalida) {
    if (str.empty()) return false;
    for (unsigned char c : str) {
        if (!isdigit(c)) return false;
    }
    try {
        valorSalida = stoi(str);
    } catch (...) {
        return false;
    }
    return valorSalida > 0;
}

static bool esExtensionTxt(const fs::path &ruta) {
    string ext = ruta.extension().string();
    for (char &c : ext) {
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return ext == ".txt";
}

static void extraerPalabrasDeLinea(const string &linea, unordered_map<string, int> &conteoLocal) {
    string palabraActual = "";
    size_t n = linea.size();

    for (size_t i = 0; i < n; ++i) {
        unsigned char c = static_cast<unsigned char>(linea[i]);

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            palabraActual += static_cast<char>(tolower(c));
        } else if (c == 0xC3 && i + 1 < n) {
            // Manejo de letras acentuadas y eñe en UTF-8 (á, é, í, ó, ú, ñ, ü)
            unsigned char sig = static_cast<unsigned char>(linea[i + 1]);
            if (sig >= 0x80 && sig <= 0x9E) {
                sig += 0x20;
            }
            if (sig == 0xA1 || sig == 0xA9 || sig == 0xAD || sig == 0xB3 ||
                sig == 0xBA || sig == 0xB1 || sig == 0xBC) {
                palabraActual += static_cast<char>(0xC3);
                palabraActual += static_cast<char>(sig);
                ++i;
            } else {
                if (!palabraActual.empty()) {
                    conteoLocal[palabraActual]++;
                    palabraActual.clear();
                }
            }
        } else {
            if (!palabraActual.empty()) {
                conteoLocal[palabraActual]++;
                palabraActual.clear();
            }
        }
    }

    if (!palabraActual.empty()) {
        conteoLocal[palabraActual]++;
    }
}

static void trabajadorIndice(int idThread, ContextoIndice *ctx) {
    while (true) {
        LibroTrabajo libroActual;
        bool tieneLibro = false;

        sem_wait(&ctx->semCola);
        if (!ctx->colaTrabajo.empty()) {
            libroActual = ctx->colaTrabajo.front();
            ctx->colaTrabajo.pop();
            tieneLibro = true;
        }
        sem_post(&ctx->semCola);

        if (!tieneLibro) {
            break;
        }

        ifstream archivoLibro(libroActual.rutaCompleta);
        if (!archivoLibro.is_open()) {
            sem_wait(&ctx->semConsola);
            cout << "[Thread " << idThread << "] Error: No se pudo leer el libro '"
                 << libroActual.nombre << "' (" << libroActual.rutaCompleta << ")." << endl;
            sem_post(&ctx->semConsola);

            sem_wait(&ctx->semIndice);
            ctx->errorEnProcesamiento = true;
            sem_post(&ctx->semIndice);
            continue;
        }

        unordered_map<string, int> conteoLocal;
        string linea;
        while (getline(archivoLibro, linea)) {
            extraerPalabrasDeLinea(linea, conteoLocal);
        }
        archivoLibro.close();

        sem_wait(&ctx->semIndice);
        for (const auto &par : conteoLocal) {
            ctx->indiceGlobal[par.first][libroActual.id] += par.second;
        }
        ctx->librosProcesados++;
        int procesadosActual = ctx->librosProcesados;
        sem_post(&ctx->semIndice);

        // Notificacion sincronizada de avance
        sem_wait(&ctx->semConsola);
        cout << "[Thread " << idThread << "] Libro procesado (ID " << libroActual.id << "): "
             << libroActual.nombre << " | Palabras unicas: " << conteoLocal.size()
             << " | Progreso: " << procesadosActual << "/" << ctx->totalLibros << endl;
        sem_post(&ctx->semConsola);
    }
}

static bool guardarMapaLibros(const string &rutaMapa, const vector<LibroTrabajo> &libros) {
    fs::path pathMapa(rutaMapa);
    if (pathMapa.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(pathMapa.parent_path(), ec);
    }

    ofstream archivoMapa(rutaMapa, ios::trunc);
    if (!archivoMapa.is_open()) {
        return false;
    }

    for (const auto &libro : libros) {
        archivoMapa << libro.id << ";" << libro.nombre << "\n";
    }
    archivoMapa.close();
    return true;
}

static bool guardarArchivoIndice(const string &rutaIndice, const map<string, map<int, int>> &indiceGlobal) {
    fs::path pathIndice(rutaIndice);
    if (pathIndice.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(pathIndice.parent_path(), ec);
    }

    ofstream archivoIndice(rutaIndice, ios::trunc);
    if (!archivoIndice.is_open()) {
        return false;
    }

    for (const auto &entradaPalabra : indiceGlobal) {
        archivoIndice << entradaPalabra.first;
        for (const auto &aparicionLibro : entradaPalabra.second) {
            archivoIndice << ";(" << aparicionLibro.first << "," << aparicionLibro.second << ")";
        }
        archivoIndice << "\n";
    }
    archivoIndice.close();
    return true;
}

void crearIndiceInvertido() {
    cout << "\n--- CREAR INDICE INVERTIDO (PARALELO) ---" << endl;

    const char* envLibrosDir = getenv("LIBROS_DIR");
    const char* envIndiceFile = getenv("INDICE_FILE");
    const char* envMapaFile = getenv("MAPA_LIBROS_FILE");
    const char* envNThreads = getenv("N_THREADS");

    if (!envLibrosDir || string(envLibrosDir).empty()) {
        cout << "Error: La variable de entorno LIBROS_DIR no esta definida o es vacia." << endl;
        return;
    }
    if (!envIndiceFile || string(envIndiceFile).empty()) {
        cout << "Error: La variable de entorno INDICE_FILE no esta definida o es vacia." << endl;
        return;
    }
    if (!envMapaFile || string(envMapaFile).empty()) {
        cout << "Error: La variable de entorno MAPA_LIBROS_FILE no esta definida o es vacia." << endl;
        return;
    }
    if (!envNThreads || string(envNThreads).empty()) {
        cout << "Error: La variable de entorno N_THREADS no esta definida o es vacia." << endl;
        return;
    }

    int nThreads = 0;
    if (!validarEnteroPositivo(string(envNThreads), nThreads)) {
        cout << "Error: El valor de N_THREADS ('" << envNThreads
             << "') no es valido. Debe ser un numero entero positivo mayor a 0." << endl;
        return;
    }

    string rutaCarpetaLibros = envLibrosDir;
    string rutaArchivoIndice = envIndiceFile;
    string rutaArchivoMapa = envMapaFile;

    std::error_code ec;
    if (!fs::exists(rutaCarpetaLibros, ec) || ec) {
        cout << "Error: La carpeta de libros '" << rutaCarpetaLibros << "' no existe." << endl;
        return;
    }
    if (!fs::is_directory(rutaCarpetaLibros, ec) || ec) {
        cout << "Error: La ruta '" << rutaCarpetaLibros << "' no corresponde a un directorio." << endl;
        return;
    }

    fs::recursive_directory_iterator itDir(
        rutaCarpetaLibros,
        fs::directory_options::skip_permission_denied,
        ec
    );
    if (ec) {
        cout << "Error: No se tienen permisos de lectura sobre la carpeta '"
             << rutaCarpetaLibros << "'." << endl;
        return;
    }

    vector<fs::path> rutasEncontradas;
    for (const auto &entrada : itDir) {
        std::error_code ecArchivo;
        if (entrada.is_regular_file(ecArchivo) && !ecArchivo && esExtensionTxt(entrada.path())) {
            rutasEncontradas.push_back(entrada.path());
        }
    }

    if (rutasEncontradas.empty()) {
        cout << "Error: No se encontraron archivos de libros (.txt) dentro de la carpeta '"
             << rutaCarpetaLibros << "'." << endl;
        return;
    }

    sort(rutasEncontradas.begin(), rutasEncontradas.end());

    vector<LibroTrabajo> listaLibros;
    int idActual = 1;
    for (const auto &ruta : rutasEncontradas) {
        ifstream pruebaLectura(ruta);
        if (!pruebaLectura.is_open()) {
            cout << "Error: El archivo '" << ruta.string()
                 << "' no puede ser leido. Abortando procesamiento." << endl;
            return;
        }
        pruebaLectura.close();

        LibroTrabajo libro;
        libro.id = idActual++;
        libro.nombre = ruta.filename().string();
        libro.rutaCompleta = ruta.string();
        listaLibros.push_back(libro);
    }

    if (!guardarMapaLibros(rutaArchivoMapa, listaLibros)) {
        cout << "Error: No se pudo crear el archivo de mapeo de libros en '"
             << rutaArchivoMapa << "'." << endl;
        return;
    }

    cout << "Carpeta verificada correctamente: " << rutaCarpetaLibros << endl;
    cout << "Total de libros identificados: " << listaLibros.size() << endl;
    cout << "Archivo de mapeo de libros generado en: " << rutaArchivoMapa << endl;
    cout << "Cantidad de threads configurados (N_THREADS): " << nThreads << endl;

    ContextoIndice ctx;
    ctx.totalLibros = static_cast<int>(listaLibros.size());
    for (const auto &libro : listaLibros) {
        ctx.colaTrabajo.push(libro);
    }

    if (sem_init(&ctx.semCola, 0, 1) != 0) {
        cout << "Error: No se pudo inicializar el semaforo de la cola de trabajo." << endl;
        return;
    }
    if (sem_init(&ctx.semIndice, 0, 1) != 0) {
        sem_destroy(&ctx.semCola);
        cout << "Error: No se pudo inicializar el semaforo del indice invertido." << endl;
        return;
    }
    if (sem_init(&ctx.semConsola, 0, 1) != 0) {
        sem_destroy(&ctx.semCola);
        sem_destroy(&ctx.semIndice);
        cout << "Error: No se pudo inicializar el semaforo de consola." << endl;
        return;
    }

    cout << "\nIniciando procesamiento paralelo..." << endl;
    vector<thread> hilos;
    hilos.reserve(nThreads);
    for (int i = 0; i < nThreads; ++i) {
        hilos.emplace_back(trabajadorIndice, i + 1, &ctx);
    }

    for (auto &hilo : hilos) {
        if (hilo.joinable()) {
            hilo.join();
        }
    }

    sem_destroy(&ctx.semCola);
    sem_destroy(&ctx.semIndice);
    sem_destroy(&ctx.semConsola);

    if (!ctx.colaTrabajo.empty() || ctx.librosProcesados != ctx.totalLibros || ctx.errorEnProcesamiento) {
        cout << "Error: El procesamiento finalizo de forma incompleta ("
             << ctx.librosProcesados << "/" << ctx.totalLibros << " libros procesados)." << endl;
        return;
    }

    cout << "\nConstruyendo archivo final de indice invertido..." << endl;
    if (!guardarArchivoIndice(rutaArchivoIndice, ctx.indiceGlobal)) {
        cout << "Error: No se pudo escribir el archivo de salida en '"
             << rutaArchivoIndice << "'." << endl;
        return;
    }

    cout << "--- Proceso Finalizado Exitosamente ---" << endl;
    cout << "Libros procesados: " << ctx.librosProcesados << "/" << ctx.totalLibros << endl;
    cout << "Total de palabras indexadas: " << ctx.indiceGlobal.size() << endl;
    cout << "Archivo de indice invertido guardado en: " << rutaArchivoIndice << endl;
}