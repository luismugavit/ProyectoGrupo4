#include <stdio.h>
#include <stdlib.h>
#include "../../headers/dispositivo.h"


void imprimirDispositivoData(dispositivo d){

    printf("ID: %d, Ruta:%s, Version_Cargada:%d", d.id,d.nombre, d.num_configs);

}
