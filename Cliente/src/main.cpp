#include <winsock2.h>
#include <iostream>
#include <string>
#include "../headers/dispositivo.h"
#include "../headers/configuracion.h"
#include "../headers/comunicacion.h"
#include "../headers/interfaz.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

Dispositivo* listaDispositivos;
int numDispositivos;
std::string nombreCliente = "Luis";

// Variables para subir cambios
Dispositivo* dispositivosNuevos = nullptr;
int numDispositivosNuevos = 0;
std::string loginUser = "";
std::string loginPass = "";


int main() {
    
    conectarServidorB();
    

    // for (int i = 0; i < numDispositivos; i++) {
    //     std::cout << "Dispositivo " << i + 1 << ": "<< listaDispositivos[i].getNombre() << std::endl;
    //     std::cout << "Dispositivo " << i + 1 << ": "<< listaDispositivos[i].getConfigs()[0].getRuta() << std::endl;
    // }
    ejecutarMenuPrincipal();

}

