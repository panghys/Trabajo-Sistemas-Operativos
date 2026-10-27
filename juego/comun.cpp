#include <sys/socket.h>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include "../include/juegoComun.h"

using namespace std;

bool leerTextoEnv(const char* clave, string &valor, string &error) {
    const char* crudo = getenv(clave);
    if (!crudo || string(crudo).empty()) {
        error = string("La variable de entorno ") + clave + " no esta definida o esta vacia.";
        return false;
    }
    valor = crudo;
    return true;
}

bool leerEnteroEnv(const char* clave, int minimo, int maximo, int &valor, string &error) {
    string texto;
    if (!leerTextoEnv(clave, texto, error)) return false;
    for (unsigned char c : texto) {
        if (!isdigit(c)) {
            error = string("El valor de ") + clave + " ('" + texto + "') no es un entero positivo valido.";
            return false;
        }
    }
    if (texto.size() > 9) {
        error = string("El valor de ") + clave + " ('" + texto + "') es demasiado grande.";
        return false;
    }
    int convertido = stoi(texto);
    if (convertido < minimo || convertido > maximo) {
        error = string("El valor de ") + clave + " (" + texto + ") debe estar entre " +
                to_string(minimo) + " y " + to_string(maximo) + ".";
        return false;
    }
    valor = convertido;
    return true;
}

bool cargarConfigServidor(ConfigJuego &config, string &error) {
    if (!leerEnteroEnv("GAME_PORT", 1024, 65535, config.puerto, error)) return false;
    if (!leerTextoEnv("GAME_LOG_FILE", config.archivoLog, error)) return false;
    if (!leerEnteroEnv("GAME_BOARD_SIZE", 10, 1000, config.tamanoTablero, error)) return false;
    if (!leerEnteroEnv("GAME_DICE_FACES", 2, 100, config.carasDado, error)) return false;
    return true;
}

bool cargarConfigCliente(ConfigJuego &config, string &error) {
    if (!leerTextoEnv("GAME_HOST", config.host, error)) return false;
    if (!leerEnteroEnv("GAME_PORT", 1024, 65535, config.puerto, error)) return false;
    return true;
}

vector<string> dividirPalabras(const string &texto) {
    vector<string> palabras;
    istringstream flujo(texto);
    string palabra;
    while (flujo >> palabra) palabras.push_back(palabra);
    return palabras;
}

string restoDespuesDePalabra(const string &texto) {
    size_t inicio = texto.find_first_not_of(" \t");
    if (inicio == string::npos) return "";
    size_t finPalabra = texto.find_first_of(" \t", inicio);
    if (finPalabra == string::npos) return "";
    size_t inicioResto = texto.find_first_not_of(" \t", finPalabra);
    if (inicioResto == string::npos) return "";
    size_t finResto = texto.find_last_not_of(" \t");
    return texto.substr(inicioResto, finResto - inicioResto + 1);
}

string aMayusculas(const string &texto) {
    string resultado = texto;
    for (char &c : resultado) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return resultado;
}

bool enviarLinea(int descriptor, const string &linea) {
    string datos = linea + "\n";
    size_t enviados = 0;
    while (enviados < datos.size()) {
        ssize_t n = send(descriptor, datos.data() + enviados, datos.size() - enviados, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        enviados += static_cast<size_t>(n);
    }
    return true;
}

LectorLineas::LectorLineas(int descriptorSocket) : descriptor(descriptorSocket) {}

int LectorLineas::recibirMas() {
    char datos[1024];
    ssize_t n;
    do {
        n = recv(descriptor, datos, sizeof(datos), 0);
    } while (n < 0 && errno == EINTR);
    if (n > 0) buffer.append(datos, static_cast<size_t>(n));
    return static_cast<int>(n);
}

bool LectorLineas::extraerLinea(string &linea) {
    size_t posicion = buffer.find('\n');
    if (posicion == string::npos) return false;
    linea = buffer.substr(0, posicion);
    buffer.erase(0, posicion + 1);
    if (!linea.empty() && linea.back() == '\r') linea.pop_back();
    if (linea.size() > largoMaximoLinea) linea.resize(largoMaximoLinea);
    return true;
}

bool LectorLineas::leerLinea(string &linea) {
    while (true) {
        if (extraerLinea(linea)) return true;
        if (buffer.size() > largoMaximoLinea) return false;
        if (recibirMas() <= 0) return false;
    }
}
