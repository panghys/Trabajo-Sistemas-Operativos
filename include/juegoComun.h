#ifndef JUEGO_COMUN_H
#define JUEGO_COMUN_H

#include <cstddef>
#include <string>
#include <vector>

struct ConfigJuego {
    std::string host;
    int puerto = 0;
    std::string archivoLog;
    int tamanoTablero = 0;
    int carasDado = 0;
};

const int minEquipos = 2;
const int maxEquipos = 4;
const int minJugadoresPorEquipo = 2;
const int maxJugadoresPorEquipo = 4;
const int largoMaximoNombre = 20;
const int largoMaximoChat = 200;
const std::size_t largoMaximoLinea = 512;

bool leerEnteroEnv(const char* clave, int minimo, int maximo, int &valor, std::string &error);
bool leerTextoEnv(const char* clave, std::string &valor, std::string &error);
bool cargarConfigServidor(ConfigJuego &config, std::string &error);
bool cargarConfigCliente(ConfigJuego &config, std::string &error);

std::vector<std::string> dividirPalabras(const std::string &texto);
std::string restoDespuesDePalabra(const std::string &texto);
std::string aMayusculas(const std::string &texto);

bool enviarLinea(int descriptor, const std::string &linea);

class LectorLineas {
public:
    explicit LectorLineas(int descriptorSocket);
    bool leerLinea(std::string &linea);
    bool extraerLinea(std::string &linea);
    int recibirMas();

private:
    int descriptor;
    std::string buffer;
};

#endif
