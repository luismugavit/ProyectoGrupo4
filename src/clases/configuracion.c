#include <stdio.h>
#include <stdlib.h>
#include "../../headers/configuracion.h"


void imprimirConfigData(configuracion cnf){

    printf("Version: %d, Ruta:: %s, Fecha:%s", cnf.version,cnf.ruta, cnf.fecha);

}
