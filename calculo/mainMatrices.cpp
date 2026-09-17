#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../include/matrices.h"

using namespace std;

void imprimirMatriz(const Matriz &m) {
    cout << fixed << setprecision(2);
    for (int i = 0; i < m.size(); i++) {
        for (int j = 0; j < m[i].size(); j++) {
            cout << m[i][j];
            if (j + 1 < m[i].size()) cout << "\t";
        }
        cout << "\n";
    }
}

int main(int argc, char *argv[]) {
    if (argc < 6) {
        cerr << "Error de argumentos." << endl;
        return 1;
    }
    string pathA = argv[1];
    string pathB = argv[2];
    string separadorStr = argv[3];
    string usuario = argv[4];
    string perfil = argv[5];

    if (usuario == "" || perfil == "") {
        cerr << "Error: El usuario y el perfil no pueden estar vacios." << endl;
        return 1;
    }
    if (separadorStr.empty()) {
        cerr << "El separador no puede ser vacio!" << endl;
        return 1;
    }
    char separador = separadorStr[0];

    cout << "===========================================" << endl;
    cout << "     MULTIPLICADOR DE MATRICES NxM         " << endl;
    cout << "===========================================" << endl;
    cout << "Usuario: " << usuario << " | Perfil: " << perfil << endl;
    cout << "-------------------------------------------" << endl;

    ResultadoLecturaMatriz lecturaA = leerMatrizDesdeArchivo(pathA, separador);
    if (!lecturaA.ok) { cerr << "Error matriz A: " << lecturaA.error << endl; return 1; }

    ResultadoLecturaMatriz lecturaB = leerMatrizDesdeArchivo(pathB, separador);
    if (!lecturaB.ok) { cerr << "Error matriz B: " << lecturaB.error << endl; return 1; }

    Matriz A = lecturaA.datos;
    Matriz B = lecturaB.datos;

    cout << "Matriz A:" << endl; imprimirMatriz(A);
    cout << "Matriz B:" << endl; imprimirMatriz(B);

    ResultadoMultiplicacion resultado = multiplicarMatrices(A, B);
    if (!resultado.ok) { cerr << "Error: " << resultado.error << endl; return 1; }

    cout << "Resultado A x B:" << endl;
    imprimirMatriz(resultado.datos);

    return 0;
}