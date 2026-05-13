#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>
#include "../headers/db.h"
#include "../headers/cliente.h"
#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"
#include "../headers/interfaz.h"

cliente* listaClientes;
int numClientes;
sqlite3 *db;

int main(){

	

	numClientes = 0;

	
	int result = sqlite3_open("db/serverconfs.sqlite", &db);
	sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
	if (result != SQLITE_OK) {
		printf("Error opening database\n");
		return result;
	}

	printf("Database opened\n") ;
	
	listaClientes = cargarBD(db, &numClientes);

	printf("CLIENTES CARGADOS: %d\n", numClientes);

   // Interfaz en consola 
    login();

    return 0;
}

