#include <iostream>
#include "../include/config.h"
#include "../include/estructuras.h"
#include "../include/funciones.h"
#include "../include/menu.h"

using namespace std;

void mostrarMenuPrincipal() {
    cout << "\n===================================" << endl;
    cout << "        SISTEMA DE GESTION         " << endl;
    cout << "===================================" << endl;
    cout << "1. Listar Usuarios" << endl;
    cout << "2. Ingresar Usuario" << endl;
    cout << "3. Eliminar Usuario por ID" << endl;
    cout << "-----------------------------------" << endl;
    cout << "4. Listar Perfiles" << endl;
    cout << "5. Ingresar / Modificar Perfil" << endl;
    cout << "6. Eliminar Perfil" << endl;
    cout << "-----------------------------------" << endl;
    cout << "0. Salir" << endl;
    cout << "===================================" << endl;
    cout << "Seleccione una opcion: ";
}

void ejecutarOpcion(int opcion, ListaUsuarios &lUsers, ListaPerfiles &lProfiles) {
    switch (opcion) {
        case 1:
            listarUsuarios(lUsers);
            break;
        case 2:
            ingresarUsuario(lUsers);
            break;
        case 3: {
            cout << "Ingrese el ID del usuario a eliminar: ";
            int id;
            cin >> id;
            eliminarUsuario(id, lUsers);
            break;
        }
        case 4:
            listarPerfiles(lProfiles);
            break;
        case 5:
            ingresarPerfil(lProfiles);
            break;
        case 6: {
            cout << "Ingrese el nombre del perfil a eliminar: ";
            string nombrePerfil;
            cin >> nombrePerfil;
            eliminarPerfil(nombrePerfil, lProfiles);
            break;
        }
        case 0:
            cout << "Saliendo del programa..." << endl;
            break;
        default:
            cout << "Opcion no valida. Intente nuevamente." << endl;
            break;
    }
}