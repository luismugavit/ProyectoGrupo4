#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H

#include "configuracion.h"

class Dispositivo {

    
public:
    int id;
    int idCliente; // Añadido para la futura PK compuesta (ID_Cliente, ID_Dispositivo)
    char nombre[100];
    Configuracion* configs;
    int num_configs;

    Dispositivo();
    Dispositivo(int id, int idCliente, const char* nombre); // Actualizado con idCliente
    Dispositivo(const Dispositivo& otro);
    Dispositivo& operator=(const Dispositivo& otro);
    ~Dispositivo();

    // Getters
    int getId() const;
    int getIdCliente() const;
    const char* getNombre() const;
    Configuracion* getConfigs() const;
    int getNumConfigs() const;

    // Setters
    void setId(int id);
    void setIdCliente(int idCliente);
    void setNombre(const char* nombre);

    // Gestión de configuraciones
    void agregarConfiguracion(const Configuracion& config);
    void mostrar() const;
};

#endif
