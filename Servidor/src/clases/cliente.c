#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../headers/cliente.h"
#include "../../headers/dispositivo.h"



void cliente_init(cliente* c, int id, const char* nombre) {

    c->id = id;
    strncpy(c->nombre, nombre, sizeof(c->nombre) - 1);
    c->nombre[sizeof(c->nombre) - 1] = '\0';

    c->numDispositivos = 0;
    

    c->listaDispositivos = (dispositivo*) malloc(sizeof(dispositivo));

    
}

void imprimirCliente(cliente c){

    printf("ID: %d, Nombre: %s, NºDispositivos:%d\n", c.id,c.nombre, c.numDispositivos);

}


