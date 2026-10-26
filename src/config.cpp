#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "../include/config.h"

using namespace std;

static string limpiarCadenaEnv(const string &str) {
    size_t inicio = str.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) return "";
    size_t fin = str.find_last_not_of(" \t\r\n");
    return str.substr(inicio, fin - inicio + 1);
}

void getEnvVariable(){
    ifstream file(".env");
    if(!file.is_open()) {
        cerr << "No se pudo obtener el archivo .env" << endl;
    } else {
        string linea;
        while(getline(file, linea)){
            size_t l = linea.find('=');
            if(l != string::npos){
                string fst = limpiarCadenaEnv(linea.substr(0, l));
                string snd = limpiarCadenaEnv(linea.substr(l + 1));
                if(!fst.empty()){
                    guardarEnv(fst.c_str(), snd.c_str(), 1);
                }
            }
        }
    }
}

int guardarEnv(const char* key, const char* valor, int overwrite){
    #ifdef _WIN32
        return _putenv_s(key, valor);
    #else
        return setenv(key, valor, overwrite);
    #endif
}

void leerArchivo(const char* path){
    if (!path) return;
    ifstream file(path);
    if(!file.is_open()){
        cout << "No se pudo abrir el archivo." << endl;
        return;
    }
    string linea;
    while(getline(file, linea)){
        cout << linea << endl;
    }
}