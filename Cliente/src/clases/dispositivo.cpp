#include "../../headers/dispositivo.h"
#include "../../headers/configuracion.h"
#include <cstring>
#include <iostream>


 Dispositivo::Dispositivo() {
        id = 0;
        nombre[0] = '\0';   
        configs = nullptr;  
        num_configs = 0;
    }

 Dispositivo::Dispositivo(int id, const char* nombre){
    this-> id = id;
    strcpy(this->nombre,nombre);

 }

// Constructor de copia profundo
Dispositivo::Dispositivo(const Dispositivo& otro) {
    id = otro.id;
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

// Operador de asignación profundo
Dispositivo& Dispositivo::operator=(const Dispositivo& otro) {
    if (this != &otro) {
        delete[] configs; // Liberar memoria previa
        id = otro.id;
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

const char* Dispositivo::getTipo() const {
    return "Dispositivo";
}



Router::Router() : Dispositivo() {
    strcpy(ip_wan, "0.0.0.0");
    strcpy(gateway, "0.0.0.0");
    num_interfaces = 2;
}

Router::Router(int id, const char* nombre) : Dispositivo(id, nombre) {
    strcpy(ip_wan, "0.0.0.0");
    strcpy(gateway, "0.0.0.0");
    num_interfaces = 2;
}

const char* Router::getTipo() const {
    return "Router";
}

void Router::mostrar() const {
    std::cout << "[Router] ID: " << id
              << " | Nombre: "   << nombre
              << " | WAN: "      << ip_wan
              << " | Gateway: "  << gateway
              << " | Versiones: " << num_configs << "\n";
}



Switch::Switch() : Dispositivo() {
    num_puertos = 24;
    strcpy(vlan_principal, "VLAN1");
}

Switch::Switch(int id, const char* nombre) : Dispositivo(id, nombre) {
    num_puertos = 24;
    strcpy(vlan_principal, "VLAN1");
}

const char* Switch::getTipo() const {
    return "Switch";
}

void Switch::mostrar() const {
    std::cout << "[Switch] ID: " << id
              << " | Nombre: "       << nombre
              << " | Puertos: "      << num_puertos
              << " | VLAN: "         << vlan_principal
              << " | Versiones: "    << num_configs << "\n";
}


PuntoAcceso::PuntoAcceso() : Dispositivo() {
    strcpy(ssid, "RED_CORP");
    strcpy(banda, "Dual");
}

PuntoAcceso::PuntoAcceso(int id, const char* nombre) : Dispositivo(id, nombre) {
    strcpy(ssid, "RED_CORP");
    strcpy(banda, "Dual");
}

const char* PuntoAcceso::getTipo() const {
    return "PuntoAcceso";
}

void PuntoAcceso::mostrar() const {
    std::cout << "[PuntoAcceso] ID: " << id
              << " | Nombre: "  << nombre
              << " | SSID: "    << ssid
              << " | Banda: "   << banda
              << " | Versiones: " << num_configs << "\n";
}
