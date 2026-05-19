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

    // Deep Copy
    Dispositivo(const Dispositivo& otro);
    Dispositivo& operator=(const Dispositivo& otro);

    // Getters
    int getId() const;
    const char* getNombre() const;
    Configuracion* getConfigs() const;
    int getNumConfigs() const;

    // Setters
    void setId(int id);
    void setNombre(const char* nombre);
    void agregarConfiguracion(const Configuracion& config);
    virtual void mostrar() const;
    virtual const char* getTipo() const;
    virtual ~Dispositivo();
};


class Router : public Dispositivo {
public:
    char ip_wan[50];
    char gateway[50];
    int  num_interfaces;

    Router();
    Router(int id, const char* nombre);

    const char* getTipo() const override;
    void mostrar()        const override;
};


class Switch : public Dispositivo {
public:
    int  num_puertos;
    char vlan_principal[20];

    Switch();
    Switch(int id, const char* nombre);

    const char* getTipo() const override;
    void mostrar()        const override;
};

class PuntoAcceso : public Dispositivo {
public:
    char ssid[50];
    char banda[10];   // "2.4GHz", "5GHz" o "Dual"

    PuntoAcceso();
    PuntoAcceso(int id, const char* nombre);

    const char* getTipo() const override;
    void mostrar()        const override;
};

#endif
