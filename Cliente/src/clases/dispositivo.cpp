#include "../../headers/dispositivo.h"
#include <cstring>
#include <iostream>

Dispositivo::Dispositivo() : id(0), idCliente(0), configs(nullptr), num_configs(0) {
    nombre[0] = '\0';
}

Dispositivo::Dispositivo(int id, int idCliente, const char* nombre) 
    : id(id), idCliente(idCliente), configs(nullptr), num_configs(0) {
    setNombre(nombre);
}

Dispositivo::Dispositivo(const Dispositivo& otro) 
    : id(otro.id), idCliente(otro.idCliente), num_configs(otro.num_configs) {
    strcpy(nombre, otro.nombre);
    
    if (num_configs > 0) {
        configs = new Configuracion[num_configs];
        for (int i = 0; i < num_configs; i++) {
            configs[i] = otro.configs[i];
        }
    } else {
        configs = nullptr;
    }
}

Dispositivo& Dispositivo::operator=(const Dispositivo& otro) {
    if (this != &otro) {
        delete[] configs;
        
        id = otro.id;
        idCliente = otro.idCliente;
        strcpy(nombre, otro.nombre);
        num_configs = otro.num_configs;
        
        if (num_configs > 0) {
            configs = new Configuracion[num_configs];
            for (int i = 0; i < num_configs; i++) {
                configs[i] = otro.configs[i];
            }
        } else {
            configs = nullptr;
        }
    }
    return *this;
}

Dispositivo::~Dispositivo() {
    delete[] configs;
}

int Dispositivo::getId() const {
    return id;
}

int Dispositivo::getIdCliente() const { 
    return idCliente; 
}

const char* Dispositivo::getNombre() const {
    return nombre;
}

Configuracion* Dispositivo::getConfigs() const {
    return configs;
}

int Dispositivo::getNumConfigs() const {
    return num_configs;
}

void Dispositivo::setId(int id) {
    this->id = id;
}

void Dispositivo::setIdCliente(int idCliente) { 
    this->idCliente = idCliente; 
}

void Dispositivo::setNombre(const char* nombre) {
    strncpy(this->nombre, nombre, 99);
    this->nombre[99] = '\0';
}

void Dispositivo::agregarConfiguracion(const Configuracion& config) {
    Configuracion* nuevo = new Configuracion[num_configs + 1];
    
    for (int i = 0; i < num_configs; i++) {
        nuevo[i] = configs[i];
    }
    nuevo[num_configs] = config;
    
    delete[] configs;
    configs = nuevo;
    num_configs++;
}

void Dispositivo::mostrar() const {
    std::cout << "Dispositivo ID: " << id << ", Nombre: " << nombre << std::endl;
    std::cout << "Configuraciones (" << num_configs << "):" << std::endl;
    for (int i = 0; i < num_configs; i++) {
        configs[i].mostrar();
    }
}
