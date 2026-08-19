#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "../include/funciones.h"
#include "../include/estructuras.h"

using namespace std;

static void reescribirArchivoPerfiles(const ListaPerfiles &lProfiles) {
    const char* path = getenv("PROFILE_FILE");
    if (!path) {
        cerr << "Error: Variable de entorno PROFILE_FILE no definida." << endl;
        return;
    }

    ofstream file(path, ios::trunc);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo para sobrescribir: " << path << endl;
        return;
    }

    for (const auto &p : lProfiles.lista) {
        file << p.nombre << ";";
        for (size_t i = 0; i < p.idsUsuarios.size(); ++i) {
            file << p.idsUsuarios[i];
            if (i + 1 < p.idsUsuarios.size()) {
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
}

void cargarPerfilesDesdeArchivo(ListaPerfiles &lProfiles) {
    const char* path = getenv("PROFILE_FILE");
    if (!path) {
        cerr << "Error: Variable de entorno PROFILE_FILE no encontrada." << endl;
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
        string nombrePerfil;
        string strIds;

        if (getline(ss, nombrePerfil, ';')) {
            Perfil p;
            p.nombre = nombrePerfil;

            if (getline(ss, strIds)) {
                stringstream ssIds(strIds);
                string idToken;
                while (getline(ssIds, idToken, ',')) {
                    if (!idToken.empty()) {
                        try {
                            p.idsUsuarios.push_back(stoi(idToken));
                        } catch (...) {}
                    }
                }
            }
            lProfiles.lista.push_back(p);
        }
    }

    file.close();
    lProfiles.cargado = true;
}

void guardarPerfilEnArchivo(const Perfil &perfil) {
    const char* path = getenv("PROFILE_FILE");
    if (!path) {
        cerr << "Error: Variable de entorno PROFILE_FILE no encontrada." << endl;
        return;
    }

    ofstream file(path, ios::app);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo para agregar perfil: " << path << endl;
        return;
    }

    file << perfil.nombre << ";";
    for (size_t i = 0; i < perfil.idsUsuarios.size(); ++i) {
        file << perfil.idsUsuarios[i];
        if (i + 1 < perfil.idsUsuarios.size()) {
            file << ",";
        }
    }
    file << "\n";

    file.close();
}

void ingresarPerfil(ListaPerfiles &lProfiles) {
    if (!lProfiles.cargado) {
        cargarPerfilesDesdeArchivo(lProfiles);
    }

    Perfil p;
    int opc = 0;
    cout << "\n--- Ingresar / Modificar Perfil ---" << endl;
    cout << "Seleccione Perfil (1. ADMIN / 2. GENERAL): ";
    cin >> opc;
    cin.ignore();

    if (opc == 1) p.nombre = "ADMIN";
    else if (opc == 2) p.nombre = "GENERAL";
    else {
        cout << "Opcion no valida." << endl;
        return;
    }

    cout << "Ingrese los IDs de usuarios asociados separados por comas (ejemplo: 0,1,2): ";
    string lineaIds;
    getline(cin, lineaIds);

    stringstream ss(lineaIds);
    string token;
    while (getline(ss, token, ',')) {
        if (!token.empty()) {
            try {
                p.idsUsuarios.push_back(stoi(token));
            } catch (...) {}
        }
    }

    int index = -1;
    for (size_t i = 0; i < lProfiles.lista.size(); ++i) {
        if (lProfiles.lista[i].nombre == p.nombre) {
            index = static_cast<int>(i);
            break;
        }
    }

    if (index != -1) {
        lProfiles.lista[index] = p;
        reescribirArchivoPerfiles(lProfiles);
        cout << "Perfil '" << p.nombre << "' actualizado con exito." << endl;
    } else {
        lProfiles.lista.push_back(p);
        guardarPerfilEnArchivo(p);
        cout << "Perfil '" << p.nombre << "' registrado con exito." << endl;
    }
}

void listarPerfiles(ListaPerfiles &lProfiles) {
    cout << "\n--- Lista de Perfiles ---" << endl;

    if (lProfiles.cargado) {
        if (lProfiles.lista.empty()) {
            cout << "No hay perfiles cargados en memoria." << endl;
            return;
        }
        for (const auto &p : lProfiles.lista) {
            cout << "Perfil: " << p.nombre << " | IDs Asociados: [ ";
            for (size_t i = 0; i < p.idsUsuarios.size(); ++i) {
                cout << p.idsUsuarios[i];
                if (i + 1 < p.idsUsuarios.size()) cout << ", ";
            }
            cout << " ]" << endl;
        }
    } else {
        const char* path = getenv("PROFILE_FILE");
        if (!path) {
            cerr << "Error: Variable PROFILE_FILE no definida." << endl;
            return;
        }

        ifstream file(path);
        if (!file.is_open()) {
            cout << "No se pudo leer el archivo de perfiles." << endl;
            return;
        }

        string linea;
        bool vacio = true;
        while (getline(file, linea)) {
            if (linea.empty()) continue;
            vacio = false;

            stringstream ss(linea);
            string nombre, strIds;
            getline(ss, nombre, ';');
            getline(ss, strIds);

            cout << "Perfil: " << nombre << " | IDs Asociados: [" << strIds << "]" << endl;
        }
        if (vacio) cout << "El archivo de perfiles esta vacio." << endl;
        file.close();
    }
}

void eliminarPerfil(const string &nombrePerfil, ListaPerfiles &lProfiles) {
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

    if (nombrePerfil == "ADMIN") {
        cout << "\n[ALERTA DE SEGURIDAD] Esta intentando eliminar el perfil ADMIN." << endl;
        cout << "Esta seguro de continuar? (1: Si / 0: No): ";
        int confirmar;
        cin >> confirmar;
        if (confirmar != 1) {
            cout << "Operacion cancelada." << endl;
            return;
        }
    }

    lProfiles.lista.erase(lProfiles.lista.begin() + indice);
    reescribirArchivoPerfiles(lProfiles);
    cout << "Perfil '" << nombrePerfil << "' eliminado correctamente." << endl;
}