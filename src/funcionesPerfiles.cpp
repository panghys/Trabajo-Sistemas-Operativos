#include <iostream>
#include <fstream>
#include <cstring>
#include "../include/funciones.h"
#include "../include/estructuras.h"

using namespace std;

static void reescribirArchivoPerfiles(const ListaPerfiles &lProfiles) {
    const char* path = getenv("PERFIL_FILE");
    if (!path) return;
    ofstream file(path, ios::trunc | ios::binary);
    for (const auto &p : lProfiles.lista) {
        file.write((const char*)&p, sizeof(Perfil));
    }
    file.close();
}

void cargarPerfilesDesdeArchivo(ListaPerfiles &lProfiles) {
    const char* path = getenv("PERFIL_FILE");
    if (!path) return;
    ifstream file(path, ios::binary);
    lProfiles.lista.clear();
    Perfil p;
    while (file.read((char*)&p, sizeof(Perfil))) {
        lProfiles.lista.push_back(p);
    }
    file.close();
    lProfiles.cargado = true;
}

void guardarPerfilEnArchivo(const Perfil &perfil) {
    const char* path = getenv("PERFIL_FILE");
    if (!path) return;
    ofstream file(path, ios::app | ios::binary);
    file.write((const char*)&perfil, sizeof(Perfil));
    file.close();
}

void ingresarPerfil(ListaPerfiles &lProfiles) {
    if (!lProfiles.cargado) cargarPerfilesDesdeArchivo(lProfiles);

    cout << "\n--- Ingresar / Modificar Perfil ---" << endl;
    string tempNombre;
    cout << "Ingrese nombre del Perfil (ej. GENERAL / ADMIN): ";
    cin >> tempNombre;

    int opcionPermitida;
    cout << "Ingrese numero de opcion de menu permitida: ";
    cin >> opcionPermitida;

    int indice = -1;
    for (size_t i = 0; i < lProfiles.lista.size(); ++i) {
        if (string(lProfiles.lista[i].nombre) == tempNombre) {
            indice = static_cast<int>(i);
            break;
        }
    }

    if (indice != -1) {
        if(lProfiles.lista[indice].numOpciones < 10) { 
             lProfiles.lista[indice].opcionesMenu[lProfiles.lista[indice].numOpciones] = opcionPermitida;
             lProfiles.lista[indice].numOpciones++;
             reescribirArchivoPerfiles(lProfiles);
             cout << "Opcion agregada al perfil existente." << endl;
        } else {
             cout << "Maximo de opciones alcanzado." << endl;
        }
    } else {
        Perfil nuevo;
        strncpy(nuevo.nombre, tempNombre.c_str(), sizeof(nuevo.nombre)-1);
        nuevo.nombre[sizeof(nuevo.nombre)-1] = '\0';
        nuevo.opcionesMenu[0] = opcionPermitida;
        nuevo.numOpciones = 1;
        
        lProfiles.lista.push_back(nuevo);
        guardarPerfilEnArchivo(nuevo);
        cout << "Nuevo perfil registrado." << endl;
    }
}

void listarPerfiles(ListaPerfiles &lProfiles) {
    if (!lProfiles.cargado) cargarPerfilesDesdeArchivo(lProfiles);
    for (const auto &p : lProfiles.lista) {
        cout << "Perfil: " << p.nombre << " | Opciones: ";
        for(int i = 0; i < p.numOpciones; i++) {
            cout << p.opcionesMenu[i] << " ";
        }
        cout << endl;
    }
}

void eliminarPerfil(const std::string &nombrePerfil, ListaPerfiles &lProfiles) {
    if (!lProfiles.cargado) cargarPerfilesDesdeArchivo(lProfiles);

    int indice = -1;
    for (size_t i = 0; i < lProfiles.lista.size(); ++i) {
        if (string(lProfiles.lista[i].nombre) == nombrePerfil) {
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
    cout << "Perfil '" << nombrePerfil << "' eliminado." << endl;
}