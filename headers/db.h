#ifndef DB_H
#define DB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cliente.h"
#include "dispositivo.h"
#include "configuracion.h"
#include "../src/sqlite3.h"


int insertClienteBD(sqlite3 *db, cliente c);
int contarFilas(sqlite3 *db, char *consulta, int bind_id);
cliente* cargarBD(sqlite3 *db, int *totalClientes);





#endif