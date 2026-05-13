#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H

#include "configuracion.h"

typedef struct {
    int id;                         
    char nombre[100];                              

    configuracion* configs;         
    int num_configs;                
    

} dispositivo;

#endif