#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

void leerArchivo(char* path);

struct User{
    public:
        int id;
        string nombre;
        string username;
        string password;
        bool profile; // 0 user, 1 admin
};

int main(){
    char* leerEnv = getenv("USER_FILE");
    leerArchivo(leerEnv);
    return 0;
}

void leerArchivo(char* path){
    ifstream file;
    cout << "test";
    file.open(path);
    if(file.is_open()){
        while (!file.eof()){
            string linea;
            getline(file,linea);
            cout << linea << endl;
        }
    }
}