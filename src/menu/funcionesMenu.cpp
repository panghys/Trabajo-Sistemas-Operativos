#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "../../include/config.h" // importante poner este por si se quiere usar las variables de entorno

using namespace std;

struct User{
    public:
        int id;
        string nombre;
        string username;
        string password;
        bool profile; // 0 user, 1 admin
};

int main(){ // esto es solo testing para probar como lee el archivo
    getEnvVariable();
    const char* leerEnv = getenv("USER_FILE");
    leerArchivo(leerEnv);
    return 0;
}
