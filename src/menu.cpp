#include <iostream>
#include <fstream>
#include <cctype>
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

void realizarConteoArchivo(const string &rutaArchivo) {
    ifstream file(rutaArchivo);
    if (!file.is_open()) {
        cout << "Error: No se pudo abrir el archivo: " << rutaArchivo << endl;
        return;
    }
    int vocales = 0, consonantes = 0, especiales = 0, palabras = 0;
    char c;
    while (file.get(c)) {
        if (isalpha(c)) {
            char lower = tolower(c);
            if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u') {
                vocales++;
            } else {
                consonantes++;
            }
        } else if (!isspace(c)) {
            especiales++; 
        }
    }
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
                cout << "Error: Acceso denegado. Solo ADMIN puede entrar." << endl;
            } else {
                cout << "\nAbriendo Administracion de Usuarios..." << endl;
                #ifdef _WIN32
                    system("admin.exe");
                #else
                    system("./admin");
                #endif
            }
            break;
        
        case 2: {
            cout << "\n--- MULTIPLICANDO MATRICES NxM ---" << endl;
            string rutaA, rutaB, separadorStr;
            cout << "Ruta Matriz A: "; cin >> rutaA;
            cout << "Ruta Matriz B: "; cin >> rutaB;
            cout << "Separador: "; cin >> separadorStr;

            #ifdef _WIN32
                string comando = "multi.exe \"" + rutaA + "\" \"" + rutaB + "\" \"" + separadorStr + "\" \"" + usuario + "\" \"" + perfil + "\"";
            #else
                string comando = "./multi \"" + rutaA + "\" \"" + rutaB + "\" \"" + separadorStr + "\" \"" + usuario + "\" \"" + perfil + "\"";
            #endif
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
                if (esPalindromo(texto)) cout << "\nResultado: SI es palindromo." << endl;
                else cout << "\nResultado: NO es palindromo." << endl;
            }
            break;
        }
        
        case 5: {
            cout << "\n--- CALCULAR f(x) = x*x + 2x + 8 ---" << endl;
            cout << "Ingrese el valor de X (numero real): ";
            double x;
            cin >> x;
            
            cout << "\nf(" << x << ") = (" << x << "*" << x << ") + (2*" << x << ") + 8" << endl;
            cout << "Resultado: " << calcularFx(x) << endl;
            
            int opcVolver = 0;
            do {
                cout << "\n1. VOLVER\nOpcion: ";
                cin >> opcVolver;
            } while (opcVolver != 1);
            break;
        }
        
        case 6: {
            cout << "\n--- CONTEO SOBRE TEXTO ---" << endl;
            cout << "Analizando archivo: " << archivo << endl;
            realizarConteoArchivo(archivo);
            
            int opcVolver = 0;
            do {
                cout << "\n1. VOLVER\nOpcion: ";
                cin >> opcVolver;
            } while (opcVolver != 1);
            break;
        }
        
        case 7: {
            cout << "\n--- CONTEO SOBRE ARCHIVO ---" << endl;
            cout << "Ruta del archivo a analizar: ";
            string ruta;
            cin >> ruta;
            realizarConteoArchivo(ruta);
            
            int opcVolver = 0;
            do {
                cout << "\n1. VOLVER\nOpcion: ";
                cin >> opcVolver;
            } while (opcVolver != 1);
            break;
        }
    }
}