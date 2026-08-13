#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// (Con este archivo se va a manejar todo lo que tiene que ver con las variables de entorno)

int guardarEnv(const char* key, const char* valor, int overwrite);
void leerArchivo(const char* path);
void getEnvVariable();

using namespace std;

/* Lo que hace esta primera función es tomar las variables de entorno del archivo
.env y las registra en el entorno real */

void getEnvVariable(){
    ifstream file(".env"); // por ahora deje ahi el .env, habría que ver donde va tipicamente si no es en la raíz
    if(!file.is_open()) cerr << "No se pudo obtener el archivo" << endl; // cerr es una cosa del iostream para manejar errores
    else{
        string linea;
        while(getline(file,linea)){
            size_t l = linea.find('='); // busca lineas del tipo (key)=(value)
            if(l != string::npos){ // si es npos significa que es null
                string fst = linea.substr(0,l); // key
                string snd = linea.substr(l+1); // value
                guardarEnv(fst.c_str(),snd.c_str(),1); // se mandan con .c_str() para que sean const char*, que lo requiere la setenv
            }
        }
    }
}

int guardarEnv(const char* key, const char* valor, int overwrite){
    #ifdef _WIN32 // si es windows, asi se registran las variables
        return _putenv_s(key,valor);
    #else // si es macos o linux, asi se hace
        return setenv(key,valor, overwrite);
    #endif
}

//      const char* testEnv = getenv("USER_FILE");
//      asi llamariamos a una variable de entorno luego

void leerArchivo(const char* path){
    ifstream file;
    file.open(path);
    if(!file.is_open()){
        cout << "No se pudo abrir el archivo." << endl;
        return;
    }
    else{
        string linea;
        while(getline(file,linea)){
            cout << linea << endl;
        }
    }
}