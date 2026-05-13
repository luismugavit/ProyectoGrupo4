#include "../../headers/configuracion.h"
#include <cstring>
#include <iostream>
Configuracion::Configuracion() : version(0) {
    ruta[0] = '\0';
    fecha[0] = '\0';
}
Configuracion::Configuracion(int version, const char* ruta, const char* fecha) 
    : version(version) {
    setRuta(ruta);
    setFecha(fecha);
}
Configuracion::~Configuracion() {}
int Configuracion::getVersion() const {
    return version;
}
const char* Configuracion::getRuta() const {
    return ruta;
}
const char* Configuracion::getFecha() const {
    return fecha;
}
void Configuracion::setVersion(int version) {
    this->version = version;
}
void Configuracion::setRuta(const char* ruta) {
    strncpy(this->ruta, ruta, 255);
    this->ruta[255] = '\0';
}
void Configuracion::setFecha(const char* fecha) {
    strncpy(this->fecha, fecha, 49);
    this->fecha[49] = '\0';
}
void Configuracion::mostrar() const {
    std::cout << "  Version: " << version 
              << ", Ruta: " << ruta 
              << ", Fecha: " << fecha << std::endl;
}