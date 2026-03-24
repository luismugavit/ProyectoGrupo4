#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include <string.h>
#include "../headers/cliente.h"
#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"
#include "../headers/interfaz.h"

int insertCliente(sqlite3 *db, cliente c) {
	sqlite3_stmt *stmt;

    //SACAR NOMBRE CLIENTE
    char name[sizeof(c.nombre)]; //esta mal
    strcpy(name, c.nombre);

    int idcl = c.id;

	char sql[] = "insert into cliente (ID, NOMBRE_CLIENTE) values (?, ?)";
	int result = sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &stmt, NULL) ;

    sqlite3_bind_int(stmt, 1, idcl);
    sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_STATIC);

	if (result != SQLITE_OK) {
		printf("Error preparing statement (INSERT)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("SQL query prepared (INSERT)\n");


	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
		printf("Error inserting new data into country table\n");
		return result;
	}

	result = sqlite3_finalize(stmt);
	if (result != SQLITE_OK) {
		printf("Error finalizing statement (INSERT)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("Prepared statement finalized (INSERT)\n");

	return SQLITE_OK;
}

int main(){

    cliente c1;
    cliente c2;
    cliente_init(&c1,1,"Luis");
    cliente_init(&c2,2,"Ander");
    imprimirCliente(c1);

    sqlite3 *db;
    int result = sqlite3_open("../db/testserverconfs.sqlite", &db);
	if (result != SQLITE_OK) {
		printf("Error opening database\n");
		return result;
	}

	printf("Database opened\n") ;

    result = insertCliente(db, c1);
    if (result != SQLITE_OK) {
		printf("Error inserting new data\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

   // Interfaz en consola 
   // MostrarMenuPrincipal(); 

    return 0;
}

