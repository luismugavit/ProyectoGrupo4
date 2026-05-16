#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

#include "dispositivo.h"

// Clase Cliente alojada aquí para centralizar la memoria dinámica y la funcionalidad
class Cliente {
public:
    int id;
    char nombre[100];
    Dispositivo* listaDispositivos;
    int numDispositivos;

    Cliente();
    Cliente(int id, const char* nombre);
    Cliente(const Cliente& otro);
    Cliente& operator=(const Cliente& otro);
    ~Cliente();

    int getId() const;
    const char* getNombre() const;
    int getNumDispositivos() const;
    
    void agregarDispositivo(const Dispositivo& d);
    int calcularNuevoIdDispositivo() const;

    // Métodos para la gestión de eliminación
    int buscarDispositivoPorId(int id) const;
    bool removerDispositivo(int id);
};

// Funcionalidades del cliente
void anyadirDispositivo(Cliente& cliente);
void listarDispositivos(const Cliente& cliente);
void eliminarDispositivo(Cliente& cliente);

#endif