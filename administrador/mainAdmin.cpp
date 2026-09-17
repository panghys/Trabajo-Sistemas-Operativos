#include <iostream>
#include "../include/config.h"
#include "../include/estructuras.h"
#include "../include/funciones.h"

using namespace std;

int main() {
    getEnvVariable();
    ListaUsuarios lUsers;
    ListaPerfiles lProfiles;
    cargarUsuariosDesdeArchivo(lUsers);
    cargarPerfilesDesdeArchivo(lProfiles);

    int subOpcion = -1;
    do {
        cout << "\n--- ADMINISTRACION DE USUARIOS Y PERFILES ---" << endl;
        cout << "1. Listar Usuarios" << endl;
        cout << "2. Ingresar Usuario" << endl;
        cout << "3. Eliminar Usuario por ID" << endl;
        cout << "-----------------------------------" << endl;
        cout << "4. Listar Perfiles" << endl;
        cout << "5. Ingresar / Modificar Perfil" << endl;
        cout << "6. Eliminar Perfil" << endl;
        cout << "-----------------------------------" << endl;
        cout << "0. Volver al Menu Principal" << endl;
        cout << "Seleccione una opcion: ";
        
        cin >> subOpcion;
        if (cin.fail()) {
            cin.clear(); cin.ignore(10000, '\n'); continue;
        }

        switch (subOpcion) {
            case 1: listarUsuarios(lUsers); break;
            case 2: ingresarUsuario(lUsers); break;
            case 3: {
                int id; cout << "Ingrese ID a eliminar: "; cin >> id;
                eliminarUsuario(id, lUsers);
                break;
            }
            case 4: listarPerfiles(lProfiles); break;
            case 5: ingresarPerfil(lProfiles); break;
            case 6: {
                string nombrePerfil; cout << "Ingrese nombre del perfil a eliminar: "; cin >> nombrePerfil;
                eliminarPerfil(nombrePerfil, lProfiles);
                break;
            }
        }
    } while (subOpcion != 0);

    return 0;
}