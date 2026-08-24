#include <iostream>
#include "../include/config.h"
#include "../include/estructuras.h"
#include "../include/funciones.h"
#include "../include/menu.h"

using namespace std;

int main() {
    // Cargar variables de entorno desde el archivo .env
    getEnvVariable();

    // Inicializar listas de usuarios y perfiles
    ListaUsuarios listaU;
    ListaPerfiles listaP;

    // Cargar datos desde archivos
    cargarUsuariosDesdeArchivo(listaU);
    cargarPerfilesDesdeArchivo(listaP);
    
    int opcion = -1;

    // Bucle principal del menu
    do {
        mostrarMenuPrincipal();
        cin >> opcion;

        // Validar entrada
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Opcion no valida. Ingrese un numero." << endl;
            continue;
        }

        ejecutarOpcion(opcion, listaU, listaP);
    } while (opcion != 0);

    return 0;
}