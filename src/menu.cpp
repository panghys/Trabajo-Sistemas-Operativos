#include <iostream>
#include "../include/config.h"
#include "../include/estructuras.h"
#include "../include/funciones.h"
#include "../include/menu.h"
#include "../include/palindromo.h"
#include "../include/fx.h"

using namespace std;

void mostrarMenuPrincipal(const std::string &usuario, const std::string &perfil) {
    cout << "\n===================================" << endl;
    cout << "        SISTEMA DE GESTION         " << endl;
    cout << "===================================" << endl;
    cout << "Usuario: " << usuario << " | Perfil: " << perfil << endl;
    cout << "=====================================" << endl;
    cout << "1. Admin de usuarios y perfiles" << endl;
    cout << "2. Multiplica matrices NxM" << endl;
    cout << "3. Juego (En construccion)" << endl;
    cout << "4. Es palindromo?" << endl;
    cout << "5. Calcular f(x) = x^2 + 2x + 8" << endl;
    cout << "6. CONTEO SOBRE TEXTO" << endl;
    cout << "7. CONTEO SOBRE ARCHIVO" << endl;
    cout << "-----------------------------------" << endl;
    cout << "0. Salir" << endl;
    cout << "===================================" << endl;
    cout << "Seleccione una opcion: ";
}

#include <fstream>
#include <cctype>

// funcion para las Opciones 6 y 7
void realizarConteoArchivo(const string &rutaArchivo) {
    ifstream file(rutaArchivo);
    if (!file.is_open()) {
        cout << "Error: No se pudo abrir el archivo: " << rutaArchivo << endl;
        return;
    }

    int vocales = 0, consonantes = 0, especiales = 0, palabras = 0;
    char c;
    
    // Contar caracteres
    while (file.get(c)) {
        if (isalpha(c)) {
            char lower = tolower(c);
            if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u') {
                vocales++;
            } else {
                consonantes++;
            }
        } else if (!isspace(c)) {
            especiales++; // no es letra ni espacio, es un caracter especial como #$%
        }
    }

    // volver al inicio del archivo para contar las palabras
    file.clear();
    file.seekg(0, ios::beg);
    string palabra;
    while (file >> palabra) {
        palabras++;
    }
    file.close();

    cout << "--- Resultados del Conteo ---" << endl;
    cout << "Vocales: " << vocales << endl;
    cout << "Consonantes: " << consonantes << endl;
    cout << "Caracteres especiales: " << especiales << endl;
    cout << "Palabras: " << palabras << endl;
}

void ejecutarOpcion(int opcion, ListaUsuarios &lUsers, ListaPerfiles &lProfiles, const std::string &usuario, const std::string &perfil, const std::string &archivo) {
    switch (opcion) {
        case 1:
            if (perfil != "ADMIN") {
                cout << "Error: Acceso denegado. Solo los usuarios con perfil ADMIN pueden entrar aqui." << endl;
            } else {
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
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Opcion no valida. Ingrese un numero." << endl;
                        continue;
                    }

                    switch (subOpcion) {
                        case 1:{
                            listarUsuarios(lUsers);
                            break;
                        }
                        
                        case 2:{
                            ingresarUsuario(lUsers);
                            break;
                        }
                        
                        case 3: {
                            cout << "Ingrese el ID del usuario a eliminar: ";
                            int id;
                            cin >> id;
                            eliminarUsuario(id, lUsers);
                            break;
                        }

                        case 4:{
                            listarPerfiles(lProfiles);
                            break;
                        }
                        
                        case 5:{
                            ingresarPerfil(lProfiles);
                            break;
                        }
                        
                        case 6: {
                            cout << "Ingrese el nombre del perfil a eliminar: ";
                            string nombrePerfil;
                            cin >> nombrePerfil;
                            eliminarPerfil(nombrePerfil, lProfiles);
                            break;
                        }

                        case 0:{
                            cout << "Volviendo al Menu Principal..." << endl;
                            break;
                        }
                        
                        default:{
                            cout << "Opcion no valida. Intente nuevamente." << endl;
                            break;
                        }
                    }
                } while (subOpcion != 0);
            }
            break;
        
            case 2: {
            cout << "\n--- MULTIPLICANDO MATRICES NxM ---" << endl;
            string rutaA, rutaB, separadorStr;
            
            // Se piden los datos necesarios al usuario
            cout << "Ingrese la ruta de la Matriz A (ej. data/MatrizA.txt): ";
            cin >> rutaA;
            cout << "Ingrese la ruta de la Matriz B (ej. data/MatrizB.txt): ";
            cin >> rutaB;
            cout << "Ingrese el caracter separador (ej. ,): ";
            cin >> separadorStr;

            // en esta parte se crea un comando para ejecutar otro programa el cual es el que multiplica las matrices multi.exe
            // orden: multi.exe <rutaA> <rutaB> <separador> <usuario> <perfil>
            // creo que si se ejecuta en linux el orden de los parametros es el mismo, solo que el ejecutable seria ./multi (sin .exe)
            string comando = "multi.exe \"" + rutaA + "\" \"" + rutaB + "\" \"" + separadorStr + "\" \"" + usuario + "\" \"" + perfil + "\"";
            
            cout << "\nLlamando al programa externo..." << endl;
            // system() ejecuta el string en la terminal del sistema operativo
            system(comando.c_str());
            break;
        }

        case 3: {
            cout << "\n--- JUEGO ---" << endl;
            cout << "Mensaje: En construccion" << endl;
            break;
        }
        
        case 4: {
            cout << "\n--- ES PALINDROMO? ---" << endl;
            cout << "1. Validar un texto" << endl;
            cout << "2. Cancelar" << endl;
            cout << "Opcion: ";
            int opcPal;
            cin >> opcPal;
            cin.ignore(); 
            
            if (opcPal == 1) {
                cout << "Ingrese el texto a validar: ";
                string texto;
                getline(cin, texto);
                
                if (esPalindromo(texto)) {
                    cout << "\nResultado: El texto SI es un palindromo." << endl;
                } else {
                    cout << "\nResultado: El texto NO es un palindromo." << endl;
                }
            } else {
                cout << "Operacion cancelada." << endl;
            }
            break;
        }
        case 5: {
            cout << "\n--- CALCULAR f(x) = x*x + 2x + 8 ---" << endl;
            cout << "Ingrese el valor de X (numero real): ";
            double x;
            cin >> x;
            
            cout << "\nEspecificacion del calculo:" << endl;
            cout << "f(" << x << ") = (" << x << "*" << x << ") + (2*" << x << ") + 8" << endl;
            cout << "Resultado: " << calcularFx(x) << endl;
            
            cout << "\nPresione Enter para VOLVER...";
            cin.ignore();
            cin.get();
            break;
        }
        case 6: {
            cout << "\n--- CONTEO SOBRE TEXTO (Archivo principal) ---" << endl;
            cout << "Analizando el archivo de origen: " << archivo << endl;
            
            realizarConteoArchivo(archivo);
            
            cout << "\nPresione Enter para VOLVER...";
            cin.ignore();
            cin.get();
            break;
        }
        case 7: {
            cout << "\n--- CONTEO SOBRE ARCHIVO ---" << endl;
            cout << "Ingrese la ruta del archivo a analizar (ej. /home/usuario/texto.txt): ";
            string ruta;
            cin >> ruta;
            
            realizarConteoArchivo(ruta);
            
            cout << "\nPresione Enter para VOLVER...";
            cin.ignore();
            cin.get();
            break;
        }
    }
}