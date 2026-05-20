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
std::string nombreCliente;


int main() {
    
    int conect = conectarServidorB();

    if(conect == 0){
        ejecutarMenuPrincipal();
    }

}

