#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "estructuras.h"
#include <string>

// --- Funciones de configuración y entorno ---
void getEnvVariable();
int guardarEnv(const char* key, const char* valor, int overwrite);
void leerArchivo(const char* path);

// --- Funciones del Menú e Interacción (Manejo de enteros) ---
void mostrarMenuPrincipal();
void ejecutarOpcion(int opcion, ListaUsuarios &lUsers, ListaPerfiles &lProfiles);

// --- Funciones CRUD de Usuarios ---
void cargarUsuariosDesdeArchivo(ListaUsuarios &lUsers);
void guardarUsuarioEnArchivo(const Usuario &user);
void ingresarUsuario(ListaUsuarios &lUsers);
void listarUsuarios(ListaUsuarios &lUsers);
void eliminarUsuario(int id, ListaUsuarios &lUsers);

// --- Funciones CRUD de Perfiles ---
void cargarPerfilesDesdeArchivo(ListaPerfiles &lProfiles);
void guardarPerfilEnArchivo(const Perfil &perfil);
void ingresarPerfil(ListaPerfiles &lProfiles);
void listarPerfiles(ListaPerfiles &lProfiles);
void eliminarPerfil(const std::string &nombrePerfil, ListaPerfiles &lProfiles);

#endif // FUNCIONES_H