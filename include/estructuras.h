#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <string>
#include <vector>

struct Usuario {
    int id;
    std::string nombre;
    std::string username;
    std::string password;
    std::string perfil; // "GENERAL" o "ADMIN"
};

struct ListaUsuarios {
    std::vector<Usuario> lista;
    bool cargado = false;
};

struct Perfil {
    std::string nombre;             // "GENERAL" o "ADMIN"
    std::vector<int> idsUsuarios;   // Arreglo de IDs asociados
};

struct ListaPerfiles {
    std::vector<Perfil> lista;
    bool cargado = false;
};

#endif