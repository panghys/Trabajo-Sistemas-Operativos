#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <string>
#include <vector>

// Estructura de Usuario con los campos solicitados
struct Usuario {
    int id;
    std::string nombre;
    std::string username;
    std::string password;
    std::string perfil; // "GENERAL" o "ADMIN"
};

// Estructura para almacenar la lista de usuarios en memoria
struct ListaUsuarios {
    std::vector<Usuario> lista;
    bool cargado = false;
};

// Estructura de Perfil: [ string, array de enteros ]
struct Perfil {
    std::string nombre;             // "GENERAL" o "ADMIN"
    std::vector<int> idsUsuarios;   // Arreglo de IDs asociados
};

// Estructura para almacenar la lista de perfiles en memoria
struct ListaPerfiles {
    std::vector<Perfil> lista;
    bool cargado = false;
};

#endif // ESTRUCTURAS_H