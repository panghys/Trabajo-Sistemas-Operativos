#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include "../include/funciones.h"
#include "../include/estructuras.h"

using namespace std;

static void reescribirArchivoUsuarios(const ListaUsuarios &lUsers) {
    const char* path = getenv("USER_FILE");
    if (!path) return;
    ofstream file(path, ios::trunc | ios::binary);
    for (const auto &u : lUsers.lista) {
        file.write((const char*)&u, sizeof(Usuario));
    }
    file.close();
}

void cargarUsuariosDesdeArchivo(ListaUsuarios &lUsers) {
    const char* path = getenv("USER_FILE");
    if (!path) return;
    ifstream file(path, ios::binary);
    lUsers.lista.clear();
    Usuario u;
    while (file.read((char*)&u, sizeof(Usuario))) {
        lUsers.lista.push_back(u);
    }
    file.close();
    lUsers.cargado = true;
}

void guardarUsuarioEnArchivo(const Usuario &user) {
    const char* path = getenv("USER_FILE");
    if (!path) return;
    ofstream file(path, ios::app | ios::binary);
    file.write((const char*)&user, sizeof(Usuario));
    file.close();
}

void ingresarUsuario(ListaUsuarios &lUsers) {
    if (!lUsers.cargado) cargarUsuariosDesdeArchivo(lUsers);

    Usuario u;
    cout << "\n--- Ingresar Nuevo Usuario ---" << endl;
    cout << "Ingrese ID: ";
    if (!(cin >> u.id)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Formato invalido." << endl;
        return;
    }
    cin.ignore();
    
    for (const auto &usr : lUsers.lista) {
        if (usr.id == u.id) {
            cout << "Error: Ya existe un usuario con ID " << u.id << endl;
            return;
        }
    }

    string temp;
    cout << "Ingrese Nombre: ";
    getline(cin, temp); 
    strncpy(u.nombre, temp.c_str(), sizeof(u.nombre)-1); 
    u.nombre[sizeof(u.nombre)-1]='\0';
    
    cout << "Ingrese Username: ";
    getline(cin, temp); 
    strncpy(u.username, temp.c_str(), sizeof(u.username)-1); 
    u.username[sizeof(u.username)-1]='\0';

    cout << "Ingrese Password: ";
    getline(cin, temp); 
    strncpy(u.password, temp.c_str(), sizeof(u.password)-1); 
    u.password[sizeof(u.password)-1]='\0';

    int opcPerfil = 0;
    while (opcPerfil != 1 && opcPerfil != 2) {
        cout << "Seleccione Perfil (1. GENERAL / 2. ADMIN): ";
        cin >> opcPerfil;
        if (opcPerfil == 1) strcpy(u.perfil, "GENERAL");
        else if (opcPerfil == 2) strcpy(u.perfil, "ADMIN");
    }
    cin.ignore();

    lUsers.lista.push_back(u);
    guardarUsuarioEnArchivo(u);
    cout << "Usuario registrado exitosamente." << endl;
}

void listarUsuarios(ListaUsuarios &lUsers) {
    if (!lUsers.cargado) cargarUsuariosDesdeArchivo(lUsers);
    cout << "\n--- Lista de Usuarios ---" << endl;
    if (lUsers.lista.empty()) {
        cout << "No hay usuarios registrados." << endl;
        return;
    }
    for (const auto &u : lUsers.lista) {
        cout << "ID: " << u.id << " | Nombre: " << u.nombre 
             << " | Username: " << u.username << " | Perfil: " << u.perfil << endl;
    }
}

void eliminarUsuario(int id, ListaUsuarios &lUsers) {
    if (!lUsers.cargado) cargarUsuariosDesdeArchivo(lUsers);
    
    int indice = -1;
    for (size_t i = 0; i < lUsers.lista.size(); ++i) {
        if (lUsers.lista[i].id == id) {
            indice = static_cast<int>(i);
            break;
        }
    }
    if (indice == -1) {
        cout << "No se encontro ningun usuario con el ID: " << id << endl;
        return;
    }
    
    if (string(lUsers.lista[indice].perfil) == "ADMIN") {
        cout << "\n[ALERTA] Intenta eliminar a un ADMIN. Desea continuar? (1: Si / 0: No): ";
        int confirmar;
        cin >> confirmar;
        if (confirmar != 1) {
            cout << "Cancelado." << endl;
            return;
        }
    }
    lUsers.lista.erase(lUsers.lista.begin() + indice);
    reescribirArchivoUsuarios(lUsers);
    cout << "Usuario eliminado correctamente." << endl;
}