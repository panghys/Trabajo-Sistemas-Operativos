#include <iostream>
#include "../include/config.h"
#include "../include/juegoComun.h"
#include "../include/juegoServidor.h"

using namespace std;

int main() {
    getEnvVariable();

    ConfigJuego config;
    string error;
    if (!cargarConfigServidor(config, error)) {
        cerr << "Error de configuración: " << error << endl;
        return 1;
    }

    instalarManejadorSenales();

    ServidorJuego servidor(config);
    if (!servidor.iniciar(error)) {
        cerr << "Error al iniciar el servidor: " << error << endl;
        return 1;
    }

    servidor.ejecutar();
    cout << "Servidor finalizado correctamente." << endl;
    return 0;
}
