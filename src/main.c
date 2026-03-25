#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>
#include "../headers/cliente.h"
#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"
#include "../headers/interfaz.h"

int insertClienteBD(sqlite3 *db, cliente c) {
	sqlite3_stmt *stmt;
	int result;
    
    char name[sizeof(c.nombre)]; 
    strcpy(name, c.nombre);

    int idcl = c.id;

    

	char sql_cliente[] = "insert into cliente (ID, NOMBRE_CLIENTE) values (?, ?)";
	result = sqlite3_prepare_v2(db, sql_cliente, strlen(sql_cliente) + 1, &stmt, NULL) ;

	if (result != SQLITE_OK) {
		printf("Error preparing statement (INSERT)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

    sqlite3_bind_int(stmt, 1, idcl);
    sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_STATIC);

	

	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
		printf("Error inserting new data into country table\n");
		return result;
	}

    

	result = sqlite3_finalize(stmt);

	printf("cliente: %s insertado (INSERT)\n", c.nombre);
	if (result != SQLITE_OK) {
		printf("Error finalizing statement (INSERT)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}


	
    for (int i = 0; i < c.numDispositivos; i++) {
        dispositivo disp = c.listaDispositivos[i];
        
        const char *sql_disp = "INSERT INTO Dispositivo (ID_DISPOSITIVO, NOMBRE_DISPOSITIVO, ID_CLIENTE) VALUES (?, ?, ?)";
        result = sqlite3_prepare_v2(db, sql_disp, -1, &stmt, NULL);
        
        if (result != SQLITE_OK) {
            printf("Error preparing statement (INSERT Dispositivo): %s\n", sqlite3_errmsg(db));
            continue;
        }

        
        sqlite3_bind_int(stmt, 1, disp.id);
        sqlite3_bind_text(stmt, 2, disp.nombre, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, c.id); 

      
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error inserting Dispositivo %d\n", disp.id);
        }
        sqlite3_finalize(stmt);
		printf("Dispositivo: %s insertado (INSERT)\n", disp.nombre);
		
        for (int j = 0; j < disp.num_configs; j++) {
            configuracion conf = disp.configs[j];

            const char *sql_conf = "INSERT INTO Configuracion (VERSION, ID_DISPOSITIVO, FECHA, RUTA) VALUES (?, ?, ?, ?)";
            result = sqlite3_prepare_v2(db, sql_conf, -1, &stmt, NULL);

            if (result != SQLITE_OK) {
                printf("Error preparing statement (INSERT Configuracion): %s\n", sqlite3_errmsg(db));
                continue;
            }

     
            sqlite3_bind_int(stmt, 1, conf.version);
            sqlite3_bind_int(stmt, 2, disp.id);
            sqlite3_bind_text(stmt, 3, conf.fecha, -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, conf.ruta, -1, SQLITE_TRANSIENT);

        
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                printf("Error inserting Configuracion v%d para Dispositivo %d\n", conf.version, disp.id);
            }

			printf("Config: %d, de Disp: %s insertado (INSERT)\n", conf.version,disp.nombre);
            sqlite3_finalize(stmt);
        }
	

	}

	return SQLITE_OK;

}

int contarFilas(sqlite3 *db, char *consulta, int bind_id){

	sqlite3_stmt *stmt;
    int count = 0;
    
    if (sqlite3_prepare_v2(db, consulta, -1, &stmt, NULL) == SQLITE_OK) {
        if (bind_id != -1) {
            sqlite3_bind_int(stmt, 1, bind_id);
        }
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count;


}

cliente* cargarBD(sqlite3 *db, int *totalClientes){
	sqlite3_stmt *stmtCliente;
	sqlite3_stmt *stmtDispo;
	sqlite3_stmt *stmtConf;

	*totalClientes = contarFilas(db, "SELECT COUNT(*) FROM CLIENTE",-1);

	cliente *listaClientes = (cliente*) malloc(*totalClientes * sizeof(cliente));

	char *sql_clientes = "SELECT ID, NOMBRE_CLIENTE FROM CLIENTE";
	sqlite3_prepare_v2(db, sql_clientes, -1, &stmtCliente, NULL);


	int i = 0;

	while(sqlite3_step(stmtCliente) == SQLITE_ROW){
		listaClientes[i].id = sqlite3_column_int(stmtCliente,0);
		strcpy(listaClientes[i].nombre, (char*)sqlite3_column_text(stmtCliente,1));
		
		char sqlcontardisp[] = "SELECT COUNT(*) FROM Dispositivo WHERE ID_CLIENTE = ?";
		listaClientes[i].numDispositivos = contarFilas(db, sqlcontardisp, listaClientes[i].id);

		if(listaClientes[i].numDispositivos > 0){
			listaClientes[i].listaDispositivos = (dispositivo*)malloc(listaClientes[i].numDispositivos*sizeof(dispositivo));
			char *sql_disp = "SELECT ID_DISPOSITIVO, NOMBRE_DISPOSITIVO FROM Dispositivo WHERE ID_CLIENTE = ?";
			sqlite3_prepare_v2(db, sql_disp, -1, &stmtDispo, NULL);
			sqlite3_bind_int(stmtDispo, 1, listaClientes[i].id);

			int j = 0;

			while (sqlite3_step(stmtDispo) == SQLITE_ROW) {
				listaClientes[i].listaDispositivos[j].id = sqlite3_column_int(stmtDispo, 0);
                strcpy(listaClientes[i].listaDispositivos[j].nombre, (const char*)sqlite3_column_text(stmtDispo, 1));

				char *sql_count_conf = "SELECT COUNT(*) FROM CONFIGURACION WHERE ID_DISPOSITIVO = ?";
                listaClientes[i].listaDispositivos[j].num_configs = contarFilas(db, sql_count_conf, listaClientes[i].listaDispositivos[j].id);

				if (listaClientes[i].listaDispositivos[j].num_configs > 0) {
					listaClientes[i].listaDispositivos[j].configs = (configuracion*) malloc(listaClientes[i].listaDispositivos[j].num_configs * sizeof(configuracion));
					char *sql_conf = "SELECT VERSION, FECHA, RUTA FROM CONFIGURACION WHERE ID_DISPOSITIVO = ?";
                    sqlite3_prepare_v2(db, sql_conf, -1, &stmtConf, NULL);
                    sqlite3_bind_int(stmtConf, 1, listaClientes[i].listaDispositivos[j].id);

					int k = 0;
                    while (sqlite3_step(stmtConf) == SQLITE_ROW) {
                        listaClientes[i].listaDispositivos[j].configs[k].version = sqlite3_column_int(stmtConf, 0);
                        strcpy(listaClientes[i].listaDispositivos[j].configs[k].fecha, (const char*)sqlite3_column_text(stmtConf, 1));
                        strcpy(listaClientes[i].listaDispositivos[j].configs[k].ruta, (const char*)sqlite3_column_text(stmtConf, 2));
                        k++;
                    }
                    sqlite3_finalize(stmtConf);
				
				}
				j++;
			}
			sqlite3_finalize(stmtDispo);
		}
		i++;
	}


	sqlite3_finalize(stmtCliente);
	return listaClientes;
}

int main(){

	sqlite3 *db;

	int numClientes = 0;
	int result = sqlite3_open("db/testserverconfs.sqlite", &db);
	if (result != SQLITE_OK) {
		printf("Error opening database\n");
		return result;
	}

	printf("Database opened\n") ;

	cliente* listaClientes = cargarBD(db, &numClientes);
	printf("primero: %d", numClientes);	
	printf("primero: %s", listaClientes[0].nombre);

	//TODO: liberar memoria de todos los MALLOC.
	//TODO: Crear funciones que carguen/inserten un cliente/dispositivo/configuracion especifica para no cargar toda la BD siempre.

   // Interfaz en consola 
   // MostrarMenuPrincipal(); 

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

