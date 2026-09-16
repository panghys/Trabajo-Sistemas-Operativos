#include <iostream>
#include "../include/config.h"
#include "../include/estructuras.h"
#include "../include/funciones.h"
#include "../include/menu.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Cargar variables de entorno desde el archivo .env
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

    // verifica que el usuario ingreso todo
    if (argUsuario.empty() || argPassword.empty() || argArchivo.empty()){
        cout << "Error: Debe ingresar usuario, password y archivo." << endl;
        cout << "Uso: ./programa -u <usuario> -p <password> -f <archivo>" << endl;
        return 1;
    }

    // Inicializar listas de usuarios y perfiles
    ListaUsuarios listaU;
    ListaPerfiles listaP;

    // Cargar datos desde archivos
    cargarUsuariosDesdeArchivo(listaU);
    cargarPerfilesDesdeArchivo(listaP);
    
    bool loginExitoso = false;
    string perfilUsuarioLogueado = "";

    for (const auto& usuario : listaU.lista) {
        if (usuario.username == argUsuario && usuario.password == argPassword) {
            loginExitoso = true;
            perfilUsuarioLogueado = usuario.perfil;
            break;
        }
    }

    // si no encuentra los datos o se ingresan daots erroneos se rechaza el acceso
    if (!loginExitoso) {
        cout << "Error: Usuario o Password incorrectos." << endl;
        return 1;
    }

    int opcion = -1;
    
    // Bucle principal del menu
    do {
        mostrarMenuPrincipal(argUsuario, perfilUsuarioLogueado);
        cin >> opcion;

        // Validar entrada
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