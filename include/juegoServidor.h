#ifndef JUEGO_SERVIDOR_H
#define JUEGO_SERVIDOR_H

#include <atomic>
#include <fstream>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include <semaphore.h>
#include "juegoComun.h"

struct Jugador {
    std::string nombre;
    int equipo = -1;
    int descriptor = -1;
    bool conectado = false;
    std::string direccion;
};

struct Equipo {
    std::string nombre;
    std::vector<int> miembros;
    int posicion = 0;
    int siguienteMiembro = 0;
};

enum class FaseJuego { espera, enCurso, terminado };

class ServidorJuego {
public:
    explicit ServidorJuego(const ConfigJuego &configuracion);
    ~ServidorJuego();
    ServidorJuego(const ServidorJuego &) = delete;
    ServidorJuego &operator=(const ServidorJuego &) = delete;

    bool iniciar(std::string &error);
    void ejecutar();

private:
    ConfigJuego config;
    int descriptorEscucha = -1;
    std::ofstream archivoLog;
    sem_t semEstado;
    sem_t semLog;
    bool semaforosListos = false;
    bool cerrado = false;
    std::vector<Jugador> jugadores;
    std::vector<Equipo> equipos;
    std::set<int> conexiones;
    std::vector<std::thread> hilosClientes;
    FaseJuego fase = FaseJuego::espera;
    std::vector<int> ordenEquipos;
    int indiceTurno = -1;
    int jugadorEnTurno = -1;
    std::mt19937 generador;
    std::atomic<bool> terminar{false};

    void registrarLog(const std::string &jugador, const std::string &equipo,
                      const std::string &accion, const std::string &detalle = "");
    void atenderCliente(int descriptor, std::string direccion);
    int procesarUnirse(int descriptor, const std::string &direccion, const std::vector<std::string> &partes);
    void procesarLanzar(int indice);
    void procesarChat(int indice, const std::string &texto);
    void procesarEstado(int indice);
    void desconectar(int descriptor, int indice, const std::string &direccion);

    void difundir(const std::string &linea);
    bool condicionInicio() const;
    std::string mensajeEspera() const;
    std::string resumenPosiciones() const;
    std::string resumenPosicionesLog() const;
    std::string nombreEquipoDe(int indice) const;
    bool equipoTieneConectados(int indiceEquipo) const;
    int contarEquiposActivos() const;
    void revisarInicio();
    void iniciarPartida();
    void siguienteTurno(bool primero, bool mantenerEquipo = false);
    void anunciarTurno(bool primero);
    void terminarPorAbandono();
    void finalizarPartida(int indiceEquipoGanador, int indiceJugadorGanador, const std::string &motivo);
    void cerrar();
};

void instalarManejadorSenales();

#endif
