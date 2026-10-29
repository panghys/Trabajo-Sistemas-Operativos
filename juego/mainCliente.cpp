#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include "../include/config.h"
#include "../include/juegoComun.h"

using namespace std;

static int conectarServidor(const string &host, int puerto, string &error) {
    addrinfo pistas;
    memset(&pistas, 0, sizeof(pistas));
    pistas.ai_family = AF_INET;
    pistas.ai_socktype = SOCK_STREAM;
    addrinfo *resultado = nullptr;
    int codigo = getaddrinfo(host.c_str(), to_string(puerto).c_str(), &pistas, &resultado);
    if (codigo != 0) {
        error = string("No se pudo resolver el host '") + host + "': " + gai_strerror(codigo);
        return -1;
    }
    int descriptor = -1;
    for (addrinfo *actual = resultado; actual != nullptr; actual = actual->ai_next) {
        descriptor = socket(actual->ai_family, actual->ai_socktype, actual->ai_protocol);
        if (descriptor < 0) continue;
        if (connect(descriptor, actual->ai_addr, actual->ai_addrlen) == 0) break;
        close(descriptor);
        descriptor = -1;
    }
    freeaddrinfo(resultado);
    if (descriptor < 0) {
        error = "No se pudo conectar con el servidor " + host + ":" + to_string(puerto) +
                " (verifique que el servidor esté iniciado).";
    }
    return descriptor;
}

static void mostrarAyuda() {
    cout << "\nComandos disponibles:" << endl;
    cout << "  lanzar        lanza el dado (solo en tu turno)" << endl;
    cout << "  chat <texto>  envía un mensaje a todos los jugadores" << endl;
    cout << "  estado        muestra posiciones y turno actual" << endl;
    cout << "  ayuda         muestra esta ayuda" << endl;
    cout << "  salir         abandona la partida" << endl;
}

static void mostrarMensajeServidor(const string &linea, const string &miNombre, bool &partidaTerminada) {
    string etiqueta = linea.substr(0, linea.find(' '));
    string resto = restoDespuesDePalabra(linea);
    if (etiqueta == "INFO" || etiqueta == "OK") {
        cout << resto << endl;
    } else if (etiqueta == "ERROR") {
        cout << "[Error] " << resto << endl;
    } else if (etiqueta == "CHAT") {
        vector<string> partes = dividirPalabras(resto);
        if (partes.size() >= 3) {
            string texto = restoDespuesDePalabra(restoDespuesDePalabra(resto));
            cout << "[" << partes[1] << "] " << partes[0] << ": " << texto << endl;
        }
    } else if (etiqueta == "TURNO") {
        vector<string> partes = dividirPalabras(resto);
        if (!partes.empty() && partes[0] == miNombre) {
            cout << ">>> Es tu turno. Escribe 'lanzar' para tirar el dado." << endl;
        }
    } else if (etiqueta == "FIN") {
        cout << "=== Partida finalizada (" << resto << ") ===" << endl;
        partidaTerminada = true;
    } else {
        cout << linea << endl;
    }
}

static bool procesarEntrada(const string &linea, int descriptor) {
    vector<string> partes = dividirPalabras(linea);
    if (partes.empty()) return true;
    string comando = aMayusculas(partes[0]);
    if (comando == "LANZAR" || comando == "L") {
        enviarLinea(descriptor, "LANZAR");
    } else if (comando == "CHAT" || comando == "C") {
        string texto = restoDespuesDePalabra(linea);
        if (texto.empty()) {
            cout << "Uso: chat <texto>" << endl;
        } else {
            enviarLinea(descriptor, "CHAT " + texto);
        }
    } else if (comando == "ESTADO") {
        enviarLinea(descriptor, "ESTADO");
    } else if (comando == "AYUDA") {
        mostrarAyuda();
    } else if (comando == "SALIR") {
        enviarLinea(descriptor, "SALIR");
        return false;
    } else {
        cout << "Comando no reconocido. Escribe 'ayuda' para ver las opciones." << endl;
    }
    return true;
}

int main() {
    getEnvVariable();
    signal(SIGPIPE, SIG_IGN);

    ConfigJuego config;
    string error;
    if (!cargarConfigCliente(config, error)) {
        cerr << "Error de configuración: " << error << endl;
        return 1;
    }

    int descriptor = conectarServidor(config.host, config.puerto, error);
    if (descriptor < 0) {
        cerr << "Error: " << error << endl;
        return 1;
    }
    cout << "Conectado al servidor " << config.host << ":" << config.puerto << endl;

    LectorLineas lector(descriptor);
    string miNombre;
    bool unido = false;
    while (!unido) {
        string equipo;
        cout << "Nombre de jugador: " << flush;
        if (!getline(cin, miNombre)) {
            close(descriptor);
            return 0;
        }
        cout << "Equipo (A-D): " << flush;
        if (!getline(cin, equipo)) {
            close(descriptor);
            return 0;
        }
        vector<string> palabrasNombre = dividirPalabras(miNombre);
        vector<string> palabrasEquipo = dividirPalabras(equipo);
        if (palabrasNombre.size() != 1 || palabrasEquipo.size() != 1) {
            cout << "[Error] Ingrese un nombre sin espacios y una letra de equipo." << endl;
            continue;
        }
        miNombre = palabrasNombre[0];
        enviarLinea(descriptor, "UNIRSE " + miNombre + " " + palabrasEquipo[0]);

        string respuesta;
        if (!lector.leerLinea(respuesta)) {
            cout << "El servidor cerró la conexion." << endl;
            close(descriptor);
            return 1;
        }
        string etiqueta = respuesta.substr(0, respuesta.find(' '));
        if (etiqueta == "OK") {
            cout << restoDespuesDePalabra(respuesta) << endl;
            unido = true;
        } else {
            cout << "[Error] " << restoDespuesDePalabra(respuesta) << endl;
        }
    }

    mostrarAyuda();

    bool activo = true;
    bool partidaTerminada = false;
    string bufferEntrada;
    string lineaServidor;
    while (lector.extraerLinea(lineaServidor)) {
        mostrarMensajeServidor(lineaServidor, miNombre, partidaTerminada);
    }

    while (activo) {
        pollfd consultas[2];
        consultas[0].fd = STDIN_FILENO;
        consultas[0].events = POLLIN;
        consultas[0].revents = 0;
        consultas[1].fd = descriptor;
        consultas[1].events = POLLIN;
        consultas[1].revents = 0;

        if (poll(consultas, 2, -1) < 0) {
            if (errno == EINTR) continue;
            break;
        }

        if (consultas[1].revents & (POLLIN | POLLHUP | POLLERR)) {
            int recibidos = lector.recibirMas();
            if (recibidos <= 0) {
                cout << "Conexión con el servidor finalizada." << endl;
                break;
            }
            while (lector.extraerLinea(lineaServidor)) {
                mostrarMensajeServidor(lineaServidor, miNombre, partidaTerminada);
            }
        }

        if (consultas[0].revents & (POLLIN | POLLHUP)) {
            char datos[512];
            ssize_t leidos = read(STDIN_FILENO, datos, sizeof(datos));
            if (leidos <= 0) {
                enviarLinea(descriptor, "SALIR");
                break;
            }
            bufferEntrada.append(datos, static_cast<size_t>(leidos));
            size_t salto;
            while (activo && (salto = bufferEntrada.find('\n')) != string::npos) {
                string linea = bufferEntrada.substr(0, salto);
                bufferEntrada.erase(0, salto + 1);
                activo = procesarEntrada(linea, descriptor);
            }
        }
    }

    close(descriptor);
    return 0;
}
