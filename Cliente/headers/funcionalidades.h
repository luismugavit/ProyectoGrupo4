#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"

#include "dispositivo.h"

#include <string>
void registrarLog(const std::string& accion);


void anadirDispositivo();
void listarDispositivos();
void eliminarDispositivo();
void anadirConfiguracion();
void mostrarHistorialDispositivo();
void verContenidoConfiguracion();
void revertirConfiguracion();

#endif