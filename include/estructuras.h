#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <vector>
#include <cstring> // Para manipular los chat

struct Usuario {
    int id;
    char nombre[50];
    char username[20];
    char password[20];
    char perfil[15]; // "GENERAL" o "ADMIN"
};

struct ListaUsuarios {
    std::vector<Usuario> lista; 
    bool cargado = false;
};

struct Perfil {
    char nombre[20];             // "GENERAL" o "ADMIN"
    int opcionesMenu[10];        // Arreglo de opciones (como son 9 puse maximo 10)
    int numOpciones;             // Cantidad real de opciones ingresadas
};

struct ListaPerfiles {
    std::vector<Perfil> lista;
    bool cargado = false;
};

#endif