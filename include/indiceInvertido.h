#ifndef INDICE_INVERTIDO_H
#define INDICE_INVERTIDO_H

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <semaphore.h>

struct LibroTrabajo {
    int id;
    std::string nombre;
    std::string rutaCompleta;
};

struct ContextoIndice {
    std::queue<LibroTrabajo> colaTrabajo;
    std::map<std::string, std::map<int, int>> indiceGlobal;
    int librosProcesados = 0;
    int totalLibros = 0;
    bool errorEnProcesamiento = false;
    sem_t semCola;
    sem_t semIndice;
    sem_t semConsola;
};

void crearIndiceInvertido();

#endif