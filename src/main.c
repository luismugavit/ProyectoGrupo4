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
	
	//TODO: LOS REGISTROS HAN DE REGISTRAR DETALLE SOBRE LAS OPERCIONES (QUE CLIENTE SE HA ANADIDO, QUE DISPOSITIVO SE HA BORRADO, ETC..)
	//TODO: liberar memoria de todos los MALLOC. Cuando se cierre el programa.
	//TODO(OPCIONAL): Cambiar datos de clientes, dispositivos, configuraciones.

//DATOS DE PRUEBA
//    //CLIENTE 1
//   	configuracion configs_r1[2];

// 	configuracion r1conf1 = {1,"/opt/r1/config_v1.cfg","2023-01-15" };
// 	configuracion r1conf2 = {2,"/opt/r1/config_v2.cfg","2023-01-15" };
    
// 	configuracion configs_r2[2];

// 	configuracion r2conf1 = {1,"/opt/r2/config_v1.cfg","2023-01-15" };
// 	configuracion r2conf2 = {2,"/opt/r2/config_v2.cfg","2023-01-15" };
    
// 	configs_r1[0] = r1conf1;
// 	configs_r1[1] = r1conf2;

// 	configs_r2[0] = r2conf1;
// 	configs_r2[1] = r2conf2;

// 	dispositivo dispos_c1[2];

// 	dispositivo r1;
// 	dispositivo r2;

// 	r1.id = 1;
// 	r2.id = 2;
// 	strcpy(r1.nombre,"R1");
// 	strcpy(r2.nombre,"R2");

// 	r1.configs = configs_r1;
// 	r2.configs = configs_r2;

// 	r1.num_configs = 2;
// 	r2.num_configs = 2;

// 	dispos_c1[0] = r1;
// 	dispos_c1[1] = r2;

	

//     cliente cliente01;
// 	cliente01.id = 2;
// 	strcpy(cliente01.nombre, "CL00");
// 	cliente01.listaDispositivos = dispos_c1;
// 	cliente01.numDispositivos = 2;

// 	//CLIENTE 2


    
// 	configs_r1[0] = r1conf1;
// 	configs_r1[1] = r1conf2;

// 	configs_r2[0] = r2conf1;
// 	configs_r2[1] = r2conf2;

// 	dispositivo dispos_c2[2];

// 	dispositivo r3;
// 	dispositivo r4;

// 	r3.id = 3;
// 	r4.id = 4;
// 	strcpy(r3.nombre,"R3");
// 	strcpy(r4.nombre,"R4");

// 	r3.configs = configs_r1;
// 	r4.configs = configs_r2;

// 	r3.num_configs = 2;
// 	r4.num_configs = 2;

// 	dispos_c2[0] = r3;
	

	

//     cliente cliente00;
// 	cliente00.id = 1;
// 	strcpy(cliente00.nombre, "CL00");
// 	cliente00.listaDispositivos = dispos_c2;
// 	cliente00.numDispositivos = 1;


// 	printf("%d",sizeof(cliente00));
// 	printf("%d",sizeof(cliente01));


// 	sqlite3 *db;
//     int result = sqlite3_open("db/testserverconfs.sqlite", &db);
// 	if (result != SQLITE_OK) {
// 		printf("Error opening database\n");
// 		return result;
// 	}

// 	printf("Database opened\n") ;

//     result = insertClienteBD(db, cliente00);
//     if (result != SQLITE_OK) {
// 		printf("Error inserting new data\n");
// 		printf("%s\n", sqlite3_errmsg(db));
// 		return result;
// 	}

    return 0;
}

