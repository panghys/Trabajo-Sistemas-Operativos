#include <iostream>
#include <cstring>
#include "../include/config.h"
#include "../include/estructuras.h"
#include "../include/funciones.h"
#include "../include/menu.h"

using namespace std;

int main(int argc, char* argv[]) {
    getEnvVariable();

    string argUsuario = "";
    string argPassword = "";
    string argArchivo = "";

    for (int i = 1; i < argc; i++){
        string arg = argv[i];
        if (arg == "-u" && i + 1 < argc){
            argUsuario = argv[++i];
        } else if (arg == "-p" && i + 1 < argc){
            argPassword = argv[++i];
        } else if (arg == "-f" && i + 1 < argc){
            argArchivo = argv[++i];
        }
    }

    if (argUsuario.empty() || argPassword.empty() || argArchivo.empty()){
        cout << "Error: Debe ingresar usuario, password y archivo." << endl;
        cout << "Uso: ./pgm -u <usuario> -p <password> -f <archivo>" << endl;
        return 1;
    }

    ListaUsuarios listaU;
    ListaPerfiles listaP;
    cargarUsuariosDesdeArchivo(listaU);
    cargarPerfilesDesdeArchivo(listaP);

    // Por si no hay ningun usuario creado
    if (listaU.lista.empty()) {
        cout << "\n[SISTEMA VACIO] Creando administrador por defecto..." << endl;
        Usuario adminUser;
        adminUser.id = 1;
        strcpy(adminUser.nombre, "Administrador");
        strcpy(adminUser.username, "admin");
        strcpy(adminUser.password, "1234");
        strcpy(adminUser.perfil, "ADMIN");
        listaU.lista.push_back(adminUser);
        guardarUsuarioEnArchivo(adminUser);
        
        cout << "Administrador creado exitosamente." << endl;
        cout << "Por favor, re-ejecute con: -u admin -p 1234 -f " << argArchivo << "\n" << endl;
        return 1;
    }
    
    bool loginExitoso = false;
    string perfilUsuarioLogueado = "";

    for (const auto& usuario : listaU.lista) {
        if (string(usuario.username) == argUsuario && string(usuario.password) == argPassword) {
            loginExitoso = true;
            perfilUsuarioLogueado = string(usuario.perfil);
            break;
        }
    }

    if (!loginExitoso) {
        cout << "Error: Usuario o Password incorrectos." << endl;
        return 1;
    }

    int opcion = -1;
    do {
        mostrarMenuPrincipal(argUsuario, perfilUsuarioLogueado);
        cin >> opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Opcion no valida. Ingrese un numero." << endl;
            continue;
        }
        ejecutarOpcion(opcion, listaU, listaP, argUsuario, perfilUsuarioLogueado, argArchivo);
    } while (opcion != 0);

    return 0;
}