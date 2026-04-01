#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/cliente.h"
#include "../headers/dispositivo.h"
#include "../headers/configuracion.h"
#include "sqlite3.h"


int insertClienteBD(sqlite3 *db, cliente c) {
	sqlite3_stmt *stmt;
	int result;
    
    char name[sizeof(c.nombre)]; 
    strcpy(name, c.nombre);

    int idcl = c.id;

    

	char sql_cliente[] = "insert into cliente (ID, NOMBRE_CLIENTE) values (?, ?)";
	result = sqlite3_prepare_v2(db, sql_cliente, strlen(sql_cliente) + 1, &stmt, NULL) ;

	if (result != SQLITE_OK) {
		printf("Error INSERT\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

    sqlite3_bind_int(stmt, 1, idcl);
    sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_STATIC);

	

	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
		printf("Error \n");
		return result;
	}

    

	result = sqlite3_finalize(stmt);

	printf("cliente: %s insertado (INSERT)\n", c.nombre);
	if (result != SQLITE_OK) {
		printf("Error\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}


	
    for (int i = 0; i < c.numDispositivos; i++) {
        dispositivo disp = c.listaDispositivos[i];
        
        const char *sql_disp = "INSERT INTO Dispositivo (ID_DISPOSITIVO, NOMBRE_DISPOSITIVO, ID_CLIENTE) VALUES (?, ?, ?)";
        result = sqlite3_prepare_v2(db, sql_disp, -1, &stmt, NULL);
        
        if (result != SQLITE_OK) {
            printf("Error (INSERT Dispositivo): %s\n", sqlite3_errmsg(db));
            continue;
        }

        
        sqlite3_bind_int(stmt, 1, disp.id);
        sqlite3_bind_text(stmt, 2, disp.nombre, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, c.id); 

      
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error Dispositivo %d\n", disp.id);
        }
        sqlite3_finalize(stmt);
		printf("Dispositivo: %s insertado (INSERT)\n", disp.nombre);
		
        for (int j = 0; j < disp.num_configs; j++) {
            configuracion conf = disp.configs[j];

            const char *sql_conf = "INSERT INTO Configuracion (VERSION, ID_DISPOSITIVO, FECHA, RUTA) VALUES (?, ?, ?, ?)";
            result = sqlite3_prepare_v2(db, sql_conf, -1, &stmt, NULL);

            if (result != SQLITE_OK) {
                printf("Error (INSERT Configuracion): %s\n", sqlite3_errmsg(db));
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

int insertDispositivoDB(sqlite3 *db, dispositivo disp, int id_cliente) {
    sqlite3_stmt *stmt;
    int result;


    const char *sql_disp = "INSERT INTO Dispositivo (ID_DISPOSITIVO, NOMBRE_DISPOSITIVO, ID_CLIENTE) VALUES (?, ?, ?)";
    result = sqlite3_prepare_v2(db, sql_disp, -1, &stmt, NULL);
    
    if (result != SQLITE_OK) {
        printf("Error (PREPARE Dispositivo): %s\n", sqlite3_errmsg(db));
        return result; 
    }


    sqlite3_bind_int(stmt, 1, disp.id);
    sqlite3_bind_text(stmt, 2, disp.nombre, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, id_cliente); 


    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("Error al insertar Dispositivo %d: %s\n", disp.id, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return result; 
    }
    
    sqlite3_finalize(stmt);
    printf("Dispositivo: %s insertado correctamente en BD.\n", disp.nombre);


    for (int j = 0; j < disp.num_configs; j++) {
        configuracion conf = disp.configs[j];

        const char *sql_conf = "INSERT INTO Configuracion (VERSION, ID_DISPOSITIVO, FECHA, RUTA) VALUES (?, ?, ?, ?)";
        result = sqlite3_prepare_v2(db, sql_conf, -1, &stmt, NULL);

        if (result != SQLITE_OK) {
            printf("Error (PREPARE Configuracion): %s\n", sqlite3_errmsg(db));
            continue; 
        }

     
        sqlite3_bind_int(stmt, 1, conf.version);
        sqlite3_bind_int(stmt, 2, disp.id);
        sqlite3_bind_text(stmt, 3, conf.fecha, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, conf.ruta, -1, SQLITE_TRANSIENT);

      
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error insertando Configuracion v%d para Dispositivo %d\n", conf.version, disp.id);
        } else {
            printf("Configuracion v%d de %s insertada (INSERT)\n", conf.version, disp.nombre);
        }

        sqlite3_finalize(stmt);
    }

    return SQLITE_OK;
}

int insertConfiguracionDB(sqlite3 *db, configuracion conf, int id_dispositivo) {
    sqlite3_stmt *stmt;
    int result;

   
    const char *sql = "INSERT INTO Configuracion (ID_DISPOSITIVO, VERSION, FECHA, RUTA) VALUES (?, ?, ?, ?);";
    
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error (PREPARE INSERT Configuracion): %s\n", sqlite3_errmsg(db));
        return result; 
    }


    sqlite3_bind_int(stmt, 1, id_dispositivo);
    

    sqlite3_bind_int(stmt, 2, conf.version);
    

    sqlite3_bind_text(stmt, 3, conf.fecha, -1, SQLITE_STATIC);
    

    sqlite3_bind_text(stmt, 4, conf.ruta, -1, SQLITE_STATIC);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("Error al insertar la Configuracion en la BD: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return result; 
    }

    sqlite3_finalize(stmt);
    
    printf("Sincronizacion completada: Configuracion v%d guardada en la Base de Datos.\n", conf.version);

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

int eliminarClienteBD(sqlite3 *db, int idCliente) {
    sqlite3_stmt *stmt;
    int result;

    char *sql = "DELETE FROM cliente WHERE ID = ?";
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        printf("Error preparando la consulta (DELETE)\n");
        return result;
    }

    sqlite3_bind_int(stmt, 1, idCliente);
    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        printf("Error borrando datos de la tabla cliente\n");
    }

    sqlite3_finalize(stmt);
    return result;
}

int eliminarDispositivoDB(sqlite3 *db, int id_dispositivo) {
    sqlite3_stmt *stmt;
    int result;

    const char *sql_disp = "DELETE FROM Dispositivo WHERE ID_DISPOSITIVO = ?";
    result = sqlite3_prepare_v2(db, sql_disp, -1, &stmt, NULL);
    
    if (result != SQLITE_OK) {
        printf("Error (PREPARE DELETE Dispositivo): %s\n", sqlite3_errmsg(db));
        return result; 
    }

    sqlite3_bind_int(stmt, 1, id_dispositivo);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("Error al eliminar Dispositivo %d de la BD: %s\n", id_dispositivo, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return result; 
    }
    
    sqlite3_finalize(stmt);
    printf("Sincronizacion completada: Dispositivo (y sus configuraciones) eliminado de la Base de Datos.\n");

    return SQLITE_OK;
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

                      
                        FILE *archivoConf = fopen(listaClientes[i].listaDispositivos[j].configs[k].ruta, "r");
                        
                        if (archivoConf == NULL) {
                            archivoConf = fopen(listaClientes[i].listaDispositivos[j].configs[k].ruta, "w");
                            
                            if (archivoConf != NULL) {
                                fclose(archivoConf);
                            } 
                               // printf("Advertencia: No se pudo crear )
                               
                            
                        } else {
                            fclose(archivoConf);
                        }
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
