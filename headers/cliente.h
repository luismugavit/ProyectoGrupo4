#ifndef CLIENTES_H
#define CLIENTES_H
#include "dispositivo.h"



typedef struct
{
    /* data */

    int id;
    char nombre[100];
    dispositivo* listaDispositivos;
    int numDispositivos;

} cliente;

void cliente_init(cliente* c, int id, const char* nombre);
void imprimirCliente(cliente c);


#endif