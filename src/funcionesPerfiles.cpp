#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "../include/funciones.h"
#include "../include/estructuras.h"

using namespace std;

//  Sobrescribe el archivo de perfiles con lo que esta en memoria
static void reescribirArchivoPerfiles(const ListaPerfiles &lProfiles) {
    const char* path = getenv("PERFIL_FILE");
    if (!path) {
        cerr << "Error: PERFIL_FILE no definido en .env" << endl;
        return;
    }

    ofstream file(path, ios::trunc);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo de perfiles para sobrescribir." << endl;
        return;
    }

    for (const auto &p : lProfiles.lista) {
        file << p.nombre << ";";
        for (size_t i = 0; i < p.opcionesMenu.size(); ++i) {
            file << p.opcionesMenu[i];
            if (i + 1 < p.opcionesMenu.size()) file << ",";
        }
        file << "\n";
    }
    file.close();
}

// Carga los perfiles desde el archivo a la memoria
void cargarPerfilesDesdeArchivo(ListaPerfiles &lProfiles) {
    const char* path = getenv("PERFIL_FILE");
    if (!path) {
        cerr << "Error: PERFIL_FILE no definido en .env" << endl;
        return;
    }

    ifstream file(path);
    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo de perfiles: " << path << endl;
        return;
    }

    lProfiles.lista.clear();
    string linea;

    while (getline(file, linea)) {
        if (linea.empty()) continue;

        stringstream ss(linea);
        string nombre, opcionesStr;

        if (getline(ss, nombre, ';') && getline(ss, opcionesStr)) {
            Perfil p;
            p.nombre = nombre;

            stringstream ssOpc(opcionesStr);
            string opcToken;
            while (getline(ssOpc, opcToken, ',')) {
                if (!opcToken.empty()) {
                    p.opcionesMenu.push_back(stoi(opcToken));
                }
            }
            lProfiles.lista.push_back(p);
        }
    }

    file.close();
    lProfiles.cargado = true;
}

// Guarda un perfil nuevo al final del archivo
void guardarPerfilEnArchivo(const Perfil &perfil) {
    const char* path = getenv("PERFIL_FILE");
    if (!path) {
        cerr << "Error: PERFIL_FILE no definido en .env" << endl;
        return;
    }

    ofstream file(path, ios::app);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo de perfiles para guardar." << endl;
        return;
    }

    file << perfil.nombre << ";";
    for (size_t i = 0; i < perfil.opcionesMenu.size(); ++i) {
        file << perfil.opcionesMenu[i];
        if (i + 1 < perfil.opcionesMenu.size()) file << ",";
    }
    file << "\n";

    file.close();
}

// Ingresa o anexa opciones permitidas a un perfil
void ingresarPerfil(ListaPerfiles &lProfiles) {
    if (!lProfiles.cargado) {
        cargarPerfilesDesdeArchivo(lProfiles);
    }

    cout << "\n--- Ingresar / Modificar Perfil ---" << endl;
    cout << "Ingrese nombre del Perfil (ej. GENERAL / ADMIN): ";
    string nombre;
    cin >> nombre;

    int opcionPermitida;
    cout << "Ingrese numero de opcion de menu permitida (ej. 1, 2, 3...): ";
    cin >> opcionPermitida;

    int indice = -1;
    for (size_t i = 0; i < lProfiles.lista.size(); ++i) {
        if (lProfiles.lista[i].nombre == nombre) {
            indice = static_cast<int>(i);
            break;
        }
    }

    if (indice != -1) {
        lProfiles.lista[indice].opcionesMenu.push_back(opcionPermitida);
        reescribirArchivoPerfiles(lProfiles);
        cout << "Opcion de menu agregada al perfil existente con exito." << endl;
    } else {
        Perfil nuevo;
        nuevo.nombre = nombre;
        nuevo.opcionesMenu.push_back(opcionPermitida);
        lProfiles.lista.push_back(nuevo);
        guardarPerfilEnArchivo(nuevo);
        cout << "Nuevo perfil registrado exitosamente." << endl;
    }
}


void listarPerfiles(ListaPerfiles &lProfiles) {
    if (lProfiles.cargado) {
        for (const auto &p : lProfiles.lista) {
            cout << p.nombre << endl;
        }
    } else {
        const char* path = getenv("PERFIL_FILE");
        if (!path) return;

        ifstream file(path);
        if (!file.is_open()) return;

        string linea;
        while (getline(file, linea)) {
            stringstream ss(linea);
            string nombre;
            if (getline(ss, nombre, ';')) {
                cout << nombre << endl;
            }
        }
        file.close();
    }
}
// Elimina un perfil por su nombre
void eliminarPerfil(const std::string &nombrePerfil, ListaPerfiles &lProfiles) {
    if (!lProfiles.cargado) {
        cargarPerfilesDesdeArchivo(lProfiles);
    }

    int indice = -1;
    for (size_t i = 0; i < lProfiles.lista.size(); ++i) {
        if (lProfiles.lista[i].nombre == nombrePerfil) {
            indice = static_cast<int>(i);
            break;
        }
    }

    if (indice == -1) {
        cout << "No se encontro el perfil: " << nombrePerfil << endl;
        return;
    }

    lProfiles.lista.erase(lProfiles.lista.begin() + indice);
    reescribirArchivoPerfiles(lProfiles);
    cout << "Perfil '" << nombrePerfil << "' eliminado correctamente." << endl;
}