#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include <string.h>
#include "../headers/cliente.h"
#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"
#include "../headers/interfaz.h"

int main(){

    cliente c1;
    cliente c2;
    cliente_init(&c1,1,"Luis");
    cliente_init(&c2,2,"Ander");
    imprimirCliente(c1);

   // Interfaz en consola 
   // MostrarMenuPrincipal(); 

    return 0;
}