#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H

#include "configuracion.h"

class Dispositivo {

    
public:
    int id;
   
    char nombre[100];
    Configuracion* configs;
    int num_configs;

    Dispositivo();
    Dispositivo(int id, const char* nombre);
    ~Dispositivo();

    // Getters
    int getId() const;
    const char* getNombre() const;
    Configuracion* getConfigs() const;
    int getNumConfigs() const;

    // Setters
    void setId(int id);
    void setNombre(const char* nombre);
    void agregarConfiguracion(const Configuracion& config);
    void mostrar() const;
};

#endif
