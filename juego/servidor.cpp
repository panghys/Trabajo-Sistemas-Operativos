#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <sstream>
#include "../include/juegoServidor.h"

using namespace std;
namespace fs = std::filesystem;

static volatile sig_atomic_t senalDetener = 0;

static void manejarSenal(int) {
    senalDetener = 1;
}

void instalarManejadorSenales() {
    struct sigaction accion;
    memset(&accion, 0, sizeof(accion));
    accion.sa_handler = manejarSenal;
    sigemptyset(&accion.sa_mask);
    sigaction(SIGINT, &accion, nullptr);
    sigaction(SIGTERM, &accion, nullptr);
    signal(SIGPIPE, SIG_IGN);
}

class Cerrojo {
public:
    explicit Cerrojo(sem_t *semaforo) : sem(semaforo) {
        while (sem_wait(sem) == -1 && errno == EINTR) {
        }
    }
    ~Cerrojo() {
        sem_post(sem);
    }
    Cerrojo(const Cerrojo &) = delete;
    Cerrojo &operator=(const Cerrojo &) = delete;

private:
    sem_t *sem;
};

static string marcaTiempo() {
    time_t ahora = time(nullptr);
    tm informacion;
    localtime_r(&ahora, &informacion);
    char texto[32];
    strftime(texto, sizeof(texto), "%Y-%m-%d %H:%M:%S", &informacion);
    return texto;
}

static string limpiarCampoLog(const string &texto) {
    string limpio = texto;
    for (char &c : limpio) {
        if (c == ';' || c == '\n' || c == '\r') c = ',';
    }
    return limpio;
}

static bool nombreValido(const string &nombre) {
    if (nombre.empty() || nombre.size() > static_cast<size_t>(largoMaximoNombre)) return false;
    for (unsigned char c : nombre) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

static int interpretarEquipo(const string &texto) {
    string mayusculas = aMayusculas(texto);
    char letra = 0;
    if (mayusculas.size() == 1) {
        letra = mayusculas[0];
    } else if (mayusculas.size() == 7 && mayusculas.compare(0, 6, "EQUIPO") == 0) {
        letra = mayusculas[6];
    }
    if (letra >= 'A' && letra < 'A' + maxEquipos) return letra - 'A';
    return -1;
}

ServidorJuego::ServidorJuego(const ConfigJuego &configuracion)
    : config(configuracion), equipos(maxEquipos), generador(random_device{}()) {
    for (int i = 0; i < maxEquipos; ++i) {
        equipos[i].nombre = string("Equipo") + static_cast<char>('A' + i);
    }
}

ServidorJuego::~ServidorJuego() {
    cerrar();
}

void ServidorJuego::registrarLog(const string &jugador, const string &equipo,
                                 const string &accion, const string &detalle) {
    Cerrojo cerrojo(&semLog);
    string linea = marcaTiempo() + ";" + limpiarCampoLog(jugador) + ";" + limpiarCampoLog(equipo) + ";" + accion;
    if (!detalle.empty()) linea += ";" + detalle;
    if (archivoLog.is_open()) {
        archivoLog << linea << "\n";
        archivoLog.flush();
    }
    cout << linea << endl;
}

bool ServidorJuego::iniciar(string &error) {
    if (sem_init(&semEstado, 0, 1) != 0 || sem_init(&semLog, 0, 1) != 0) {
        error = "No se pudieron inicializar los semaforos del servidor.";
        return false;
    }
    semaforosListos = true;

    fs::path rutaLog(config.archivoLog);
    if (rutaLog.has_parent_path()) {
        error_code codigo;
        fs::create_directories(rutaLog.parent_path(), codigo);
    }
    archivoLog.open(config.archivoLog, ios::app);
    if (!archivoLog.is_open()) {
        error = "No se pudo abrir el archivo de log '" + config.archivoLog + "'.";
        return false;
    }

    descriptorEscucha = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptorEscucha < 0) {
        error = string("No se pudo crear el socket: ") + strerror(errno);
        return false;
    }
    int activar = 1;
    setsockopt(descriptorEscucha, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));

    sockaddr_in direccion;
    memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);
    direccion.sin_port = htons(static_cast<uint16_t>(config.puerto));
    if (bind(descriptorEscucha, reinterpret_cast<sockaddr *>(&direccion), sizeof(direccion)) < 0) {
        error = "No se pudo usar el puerto " + to_string(config.puerto) + ": " + strerror(errno);
        return false;
    }
    if (listen(descriptorEscucha, 16) < 0) {
        error = string("No se pudo escuchar conexiones: ") + strerror(errno);
        return false;
    }

    registrarLog("SERVIDOR", "-", "INICIA_SERVIDOR",
                 "puerto=" + to_string(config.puerto) + " tablero=" + to_string(config.tamanoTablero) +
                 " dado=" + to_string(config.carasDado));
    return true;
}

void ServidorJuego::ejecutar() {
    cout << "Servidor escuchando en el puerto " << config.puerto << ". Esperando jugadores..." << endl;
    while (!terminar && !senalDetener) {
        pollfd consulta;
        consulta.fd = descriptorEscucha;
        consulta.events = POLLIN;
        consulta.revents = 0;
        int resultado = poll(&consulta, 1, 300);
        if (resultado <= 0) continue;

        sockaddr_in origen;
        socklen_t largoOrigen = sizeof(origen);
        int descriptor = accept(descriptorEscucha, reinterpret_cast<sockaddr *>(&origen), &largoOrigen);
        if (descriptor < 0) continue;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &origen.sin_addr, ip, sizeof(ip));
        string direccion = string(ip) + ":" + to_string(ntohs(origen.sin_port));

        {
            Cerrojo cerrojo(&semEstado);
            conexiones.insert(descriptor);
        }
        try {
            hilosClientes.emplace_back(&ServidorJuego::atenderCliente, this, descriptor, direccion);
        } catch (const system_error &) {
            Cerrojo cerrojo(&semEstado);
            conexiones.erase(descriptor);
            close(descriptor);
        }
    }

    if (senalDetener) {
        Cerrojo cerrojo(&semEstado);
        if (fase != FaseJuego::terminado) {
            difundir("INFO El servidor se esta deteniendo");
            difundir("FIN SIN_GANADOR");
            registrarLog("SERVIDOR", "-", "TERMINA_PARTIDA", "servidor detenido por senal");
            fase = FaseJuego::terminado;
        }
    }
    cerrar();
}

void ServidorJuego::cerrar() {
    if (cerrado) return;
    cerrado = true;
    terminar = true;

    this_thread::sleep_for(chrono::milliseconds(200));
    if (semaforosListos) {
        Cerrojo cerrojo(&semEstado);
        for (int descriptor : conexiones) {
            shutdown(descriptor, SHUT_RDWR);
        }
    }
    for (auto &hilo : hilosClientes) {
        if (hilo.joinable()) hilo.join();
    }
    hilosClientes.clear();

    if (descriptorEscucha >= 0) {
        close(descriptorEscucha);
        descriptorEscucha = -1;
    }
    if (semaforosListos) {
        registrarLog("SERVIDOR", "-", "CIERRA_SERVIDOR", "recursos liberados");
    }
    if (archivoLog.is_open()) archivoLog.close();
    if (semaforosListos) {
        sem_destroy(&semEstado);
        sem_destroy(&semLog);
        semaforosListos = false;
    }
}

void ServidorJuego::difundir(const string &linea) {
    for (const auto &jugador : jugadores) {
        if (jugador.conectado) enviarLinea(jugador.descriptor, linea);
    }
}

string ServidorJuego::nombreEquipoDe(int indice) const {
    if (indice < 0 || indice >= static_cast<int>(jugadores.size())) return "-";
    int equipo = jugadores[indice].equipo;
    if (equipo < 0) return "-";
    return equipos[equipo].nombre;
}

bool ServidorJuego::equipoTieneConectados(int indiceEquipo) const {
    for (int miembro : equipos[indiceEquipo].miembros) {
        if (jugadores[miembro].conectado) return true;
    }
    return false;
}

int ServidorJuego::contarEquiposActivos() const {
    int activos = 0;
    for (int indiceEquipo : ordenEquipos) {
        if (equipoTieneConectados(indiceEquipo)) activos++;
    }
    return activos;
}

bool ServidorJuego::condicionInicio() const {
    int equiposConJugadores = 0;
    for (const auto &equipo : equipos) {
        if (equipo.miembros.empty()) continue;
        if (static_cast<int>(equipo.miembros.size()) < minJugadoresPorEquipo) return false;
        equiposConJugadores++;
    }
    return equiposConJugadores >= minEquipos;
}

string ServidorJuego::mensajeEspera() const {
    int equiposConJugadores = 0;
    ostringstream composicion;
    vector<string> faltantes;
    for (const auto &equipo : equipos) {
        if (equipo.miembros.empty()) continue;
        if (equiposConJugadores > 0) composicion << ", ";
        composicion << equipo.nombre << "=" << equipo.miembros.size();
        equiposConJugadores++;
        int cantidad = static_cast<int>(equipo.miembros.size());
        if (cantidad < minJugadoresPorEquipo) {
            faltantes.push_back(equipo.nombre + " necesita " +
                                to_string(minJugadoresPorEquipo - cantidad) + " jugador(es) mas");
        }
    }
    if (equiposConJugadores < minEquipos) {
        faltantes.push_back("faltan " + to_string(minEquipos - equiposConJugadores) + " equipo(s)");
    }
    ostringstream mensaje;
    mensaje << "Esperando jugadores (inicio: minimo " << minEquipos << " equipos y " << minJugadoresPorEquipo
            << " jugadores por equipo). Equipos: " << (equiposConJugadores ? composicion.str() : string("ninguno"))
            << ". Falta: ";
    for (size_t i = 0; i < faltantes.size(); ++i) {
        if (i > 0) mensaje << "; ";
        mensaje << faltantes[i];
    }
    return mensaje.str();
}

string ServidorJuego::resumenPosiciones() const {
    ostringstream resumen;
    bool primero = true;
    for (const auto &equipo : equipos) {
        if (equipo.miembros.empty()) continue;
        if (!primero) resumen << " | ";
        resumen << equipo.nombre << " " << equipo.posicion << "/" << config.tamanoTablero;
        primero = false;
    }
    return resumen.str();
}

string ServidorJuego::resumenPosicionesLog() const {
    ostringstream resumen;
    bool primero = true;
    for (const auto &equipo : equipos) {
        if (equipo.miembros.empty()) continue;
        if (!primero) resumen << ",";
        resumen << equipo.nombre << "=" << equipo.posicion;
        primero = false;
    }
    return resumen.str();
}

void ServidorJuego::revisarInicio() {
    if (fase != FaseJuego::espera) return;
    if (condicionInicio()) {
        iniciarPartida();
    } else {
        difundir("INFO " + mensajeEspera());
    }
}

void ServidorJuego::iniciarPartida() {
    fase = FaseJuego::enCurso;
    ordenEquipos.clear();
    for (int i = 0; i < maxEquipos; ++i) {
        if (!equipos[i].miembros.empty()) {
            ordenEquipos.push_back(i);
            equipos[i].posicion = 0;
            equipos[i].siguienteMiembro = 0;
        }
    }
    ostringstream detalle;
    for (size_t i = 0; i < ordenEquipos.size(); ++i) {
        if (i > 0) detalle << ",";
        detalle << equipos[ordenEquipos[i]].nombre << "=" << equipos[ordenEquipos[i]].miembros.size();
    }
    registrarLog("SERVIDOR", "-", "INICIA_PARTIDA", detalle.str());
    difundir("INFO La partida comienza. Tablero de " + to_string(config.tamanoTablero) +
             " posiciones, dado de " + to_string(config.carasDado) + " caras.");
    difundir("INFO Equipos en juego: " + detalle.str());
    indiceTurno = -1;
    siguienteTurno(true);
}

void ServidorJuego::anunciarTurno(bool primero) {
    const Jugador &jugador = jugadores[jugadorEnTurno];
    string equipo = equipos[jugador.equipo].nombre;
    registrarLog(jugador.nombre, equipo, primero ? "INICIA_TURNO" : "CAMBIA_TURNO");
    difundir(string("INFO ") + (primero ? "Turno de jugador: " : "Siguiente jugador: ") + jugador.nombre +
             " (" + equipo + ")");
    difundir("TURNO " + jugador.nombre + " " + equipo);
}

void ServidorJuego::siguienteTurno(bool primero) {
    if (contarEquiposActivos() < 2) {
        terminarPorAbandono();
        return;
    }
    int cantidadEquipos = static_cast<int>(ordenEquipos.size());
    for (int intento = 0; intento < cantidadEquipos; ++intento) {
        indiceTurno = (indiceTurno + 1) % cantidadEquipos;
        Equipo &equipo = equipos[ordenEquipos[indiceTurno]];
        int total = static_cast<int>(equipo.miembros.size());
        for (int k = 0; k < total; ++k) {
            int posicionMiembro = (equipo.siguienteMiembro + k) % total;
            int candidato = equipo.miembros[posicionMiembro];
            if (jugadores[candidato].conectado) {
                equipo.siguienteMiembro = (posicionMiembro + 1) % total;
                jugadorEnTurno = candidato;
                anunciarTurno(primero);
                return;
            }
        }
    }
    terminarPorAbandono();
}

void ServidorJuego::terminarPorAbandono() {
    int ganador = -1;
    int activos = 0;
    for (int indiceEquipo : ordenEquipos) {
        if (equipoTieneConectados(indiceEquipo)) {
            ganador = indiceEquipo;
            activos++;
        }
    }
    if (activos != 1) ganador = -1;
    finalizarPartida(ganador, -1, "los demas equipos abandonaron la partida");
}

void ServidorJuego::finalizarPartida(int indiceEquipoGanador, int indiceJugadorGanador, const string &motivo) {
    fase = FaseJuego::terminado;
    jugadorEnTurno = -1;
    if (indiceEquipoGanador >= 0) {
        string equipo = equipos[indiceEquipoGanador].nombre;
        string quien = indiceJugadorGanador >= 0 ? jugadores[indiceJugadorGanador].nombre : "SERVIDOR";
        registrarLog(quien, equipo, "GANA", motivo);
    }
    registrarLog("SERVIDOR", "-", "TERMINA_PARTIDA", motivo);
    registrarLog("SERVIDOR", "-", "RESULTADO_FINAL", resumenPosicionesLog());
    difundir("INFO Fin de la partida: " + motivo);
    if (indiceEquipoGanador >= 0) {
        difundir("INFO Equipo ganador: " + equipos[indiceEquipoGanador].nombre);
    } else {
        difundir("INFO La partida termino sin equipo ganador");
    }
    difundir("INFO Posiciones finales: " + resumenPosiciones());
    difundir(string("FIN ") + (indiceEquipoGanador >= 0 ? equipos[indiceEquipoGanador].nombre : string("SIN_GANADOR")));
    terminar = true;
}

int ServidorJuego::procesarUnirse(int descriptor, const string &direccion, const vector<string> &partes) {
    Cerrojo cerrojo(&semEstado);
    if (fase != FaseJuego::espera) {
        enviarLinea(descriptor, "ERROR Partida en curso, no se aceptan nuevos jugadores");
        registrarLog("-", "-", "RECHAZA_CONEXION", direccion + " partida en curso");
        return -2;
    }
    if (partes.size() != 3) {
        enviarLinea(descriptor, "ERROR Uso: UNIRSE <nombre> <equipo>");
        return -1;
    }
    const string &nombre = partes[1];
    if (!nombreValido(nombre)) {
        enviarLinea(descriptor, "ERROR Nombre invalido: use de 1 a " + to_string(largoMaximoNombre) +
                                " caracteres (letras, numeros o _)");
        return -1;
    }
    for (const auto &jugador : jugadores) {
        if (jugador.conectado && jugador.nombre == nombre) {
            enviarLinea(descriptor, "ERROR Ya existe un jugador conectado con ese nombre");
            return -1;
        }
    }
    int indiceEquipo = interpretarEquipo(partes[2]);
    if (indiceEquipo < 0) {
        enviarLinea(descriptor, string("ERROR Equipo invalido: use letras de A a ") +
                                static_cast<char>('A' + maxEquipos - 1));
        return -1;
    }
    if (static_cast<int>(equipos[indiceEquipo].miembros.size()) >= maxJugadoresPorEquipo) {
        enviarLinea(descriptor, "ERROR El " + equipos[indiceEquipo].nombre + " esta lleno");
        return -1;
    }

    Jugador nuevo;
    nuevo.nombre = nombre;
    nuevo.equipo = indiceEquipo;
    nuevo.descriptor = descriptor;
    nuevo.conectado = true;
    nuevo.direccion = direccion;
    jugadores.push_back(nuevo);
    int indice = static_cast<int>(jugadores.size()) - 1;
    equipos[indiceEquipo].miembros.push_back(indice);

    string nombreEquipo = equipos[indiceEquipo].nombre;
    enviarLinea(descriptor, "OK Bienvenido " + nombre + ", perteneces al " + nombreEquipo);
    registrarLog(nombre, nombreEquipo, "CONECTA", direccion);
    registrarLog(nombre, nombreEquipo, "UNE_EQUIPO", nombreEquipo);
    difundir("INFO " + nombre + " se unio al " + nombreEquipo);
    revisarInicio();
    return indice;
}

void ServidorJuego::procesarLanzar(int indice) {
    Cerrojo cerrojo(&semEstado);
    const Jugador &jugador = jugadores[indice];
    string equipo = equipos[jugador.equipo].nombre;
    if (fase == FaseJuego::espera) {
        enviarLinea(jugador.descriptor, "ERROR La partida aun no comienza");
        registrarLog(jugador.nombre, equipo, "ACCION_RECHAZADA", "LANZAR antes del inicio");
        return;
    }
    if (fase == FaseJuego::terminado) {
        enviarLinea(jugador.descriptor, "ERROR La partida ya termino");
        return;
    }
    if (indice != jugadorEnTurno) {
        enviarLinea(jugador.descriptor, "ERROR No es tu turno, le corresponde a " + jugadores[jugadorEnTurno].nombre);
        registrarLog(jugador.nombre, equipo, "ACCION_RECHAZADA", "LANZAR fuera de turno");
        return;
    }

    uniform_int_distribution<int> dado(1, config.carasDado);
    int valor = dado(generador);
    Equipo &equipoActual = equipos[jugador.equipo];
    int desde = equipoActual.posicion;
    int hasta = desde + valor;
    equipoActual.posicion = hasta;

    registrarLog(jugador.nombre, equipo, "LANZA_DADO", to_string(valor));
    registrarLog(jugador.nombre, equipo, "AVANZA", to_string(desde) + ";" + to_string(hasta));
    difundir("INFO " + jugador.nombre + " obtuvo: " + to_string(valor));
    difundir("INFO " + equipo + " avanzo hasta la posicion " + to_string(hasta));
    difundir("INFO Posiciones: " + resumenPosiciones());

    if (hasta >= config.tamanoTablero) {
        finalizarPartida(jugador.equipo, indice, "el " + equipo + " alcanzo la posicion " + to_string(hasta) +
                                                  " (meta: " + to_string(config.tamanoTablero) + ")");
    } else {
        siguienteTurno(false);
    }
}

void ServidorJuego::procesarChat(int indice, const string &texto) {
    Cerrojo cerrojo(&semEstado);
    const Jugador &jugador = jugadores[indice];
    if (texto.empty()) {
        enviarLinea(jugador.descriptor, "ERROR Uso: CHAT <mensaje>");
        return;
    }
    string recortado = texto.substr(0, static_cast<size_t>(largoMaximoChat));
    string equipo = equipos[jugador.equipo].nombre;
    registrarLog(jugador.nombre, equipo, "CHAT", limpiarCampoLog(recortado));
    difundir("CHAT " + jugador.nombre + " " + equipo + " " + recortado);
}

void ServidorJuego::procesarEstado(int indice) {
    Cerrojo cerrojo(&semEstado);
    int descriptor = jugadores[indice].descriptor;
    if (fase == FaseJuego::espera) {
        enviarLinea(descriptor, "INFO " + mensajeEspera());
        return;
    }
    enviarLinea(descriptor, "INFO Posiciones: " + resumenPosiciones());
    if (fase == FaseJuego::enCurso) {
        enviarLinea(descriptor, "INFO Turno de: " + jugadores[jugadorEnTurno].nombre + " (" +
                                nombreEquipoDe(jugadorEnTurno) + ")");
    } else {
        enviarLinea(descriptor, "INFO La partida ya termino");
    }
}

void ServidorJuego::desconectar(int descriptor, int indice, const string &direccion) {
    Cerrojo cerrojo(&semEstado);
    if (indice >= 0 && jugadores[indice].conectado) {
        Jugador &jugador = jugadores[indice];
        string equipo = equipos[jugador.equipo].nombre;
        jugador.conectado = false;
        jugador.descriptor = -1;
        registrarLog(jugador.nombre, equipo, "DESCONECTA", direccion);

        if (fase == FaseJuego::espera) {
            vector<int> &miembros = equipos[jugador.equipo].miembros;
            for (size_t i = 0; i < miembros.size(); ++i) {
                if (miembros[i] == indice) {
                    miembros.erase(miembros.begin() + static_cast<long>(i));
                    break;
                }
            }
            difundir("INFO " + jugador.nombre + " se desconecto");
            difundir("INFO " + mensajeEspera());
        } else if (fase == FaseJuego::enCurso) {
            difundir("INFO " + jugador.nombre + " se desconecto");
            if (indice == jugadorEnTurno) {
                siguienteTurno(false);
            } else if (contarEquiposActivos() < 2) {
                terminarPorAbandono();
            }
        }
    } else if (indice < 0) {
        registrarLog("-", "-", "CONEXION_CERRADA", direccion);
    }
    conexiones.erase(descriptor);
    close(descriptor);
}

void ServidorJuego::atenderCliente(int descriptor, string direccion) {
    LectorLineas lector(descriptor);
    int indiceJugador = -1;
    string linea;
    while (lector.leerLinea(linea)) {
        vector<string> partes = dividirPalabras(linea);
        if (partes.empty()) continue;
        string comando = aMayusculas(partes[0]);

        if (indiceJugador < 0) {
            if (comando == "UNIRSE") {
                int resultado = procesarUnirse(descriptor, direccion, partes);
                if (resultado == -2) break;
                if (resultado >= 0) indiceJugador = resultado;
            } else if (comando == "SALIR") {
                break;
            } else {
                enviarLinea(descriptor, "ERROR Primero debe unirse con: UNIRSE <nombre> <equipo>");
            }
            continue;
        }

        if (comando == "LANZAR") {
            procesarLanzar(indiceJugador);
        } else if (comando == "CHAT") {
            procesarChat(indiceJugador, restoDespuesDePalabra(linea));
        } else if (comando == "ESTADO") {
            procesarEstado(indiceJugador);
        } else if (comando == "SALIR") {
            break;
        } else {
            Cerrojo cerrojo(&semEstado);
            enviarLinea(descriptor, "ERROR Comando desconocido: " + partes[0]);
        }
    }
    desconectar(descriptor, indiceJugador, direccion);
}
