#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include "sqlite3.h"
#include "../headers/db.h"
#include "../headers/cliente.h"
#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"

extern cliente* listaClientes;
extern int numClientes;
extern sqlite3 *db;

char usuario[10];




void registrarOperacion(const char *usuario, const char *operacion) {
    
    FILE *archivoLog = fopen("registros.txt", "a");
    
    if (archivoLog == NULL) {
        printf("Error: No se pudo escribir en el archivo de registros.\n");
        return; 
    }

 
    time_t t = time(NULL);
    struct tm tiempoLocal = *localtime(&t);

    fprintf(archivoLog, "%s, %s, %02d/%02d/%d %02d:%02d:%02d\n", 
            usuario, 
            operacion,
            tiempoLocal.tm_mday, 
            tiempoLocal.tm_mon + 1,     
            tiempoLocal.tm_year + 1900,  
            tiempoLocal.tm_hour, 
            tiempoLocal.tm_min, 
            tiempoLocal.tm_sec);

 
    fclose(archivoLog);
}

void infoCliente(cliente c){
    printf("\n===========================================\n");
    printf("           INFO DEL CLIENTE\n");
    printf("===========================================\n");
    printf(" Nombre: %s\n", c.nombre);
    printf(" ID: %d\n", c.id);
    printf("===========================================\n\n");

    printf("--- Dispositivos del Cliente ---\n");
    printf("ID / Nombre / Version Actual / Fecha\n");
    printf("-------------------------------------------\n");

    if (c.numDispositivos == 0 || c.listaDispositivos == NULL) {
        printf("Este cliente no tiene dispositivos registrados.\n");
    } else {
        for (int i = 0; i < c.numDispositivos; i++) {
            dispositivo d = c.listaDispositivos[i];
            
            if (d.num_configs > 0 && d.configs != NULL) {
                int indice_ultima = d.num_configs - 1;
                configuracion ultima_conf = d.configs[indice_ultima];
                
                printf("%d / %s / v%d / %s\n", 
                       d.id, 
                       d.nombre, 
                       ultima_conf.version, 
                       ultima_conf.fecha);
            } else {
                printf("%d / %s / Sin config / ---\n", 
                       d.id, 
                       d.nombre);
            }
        }
    }
}


void ListarClientes() {
    printf("\n--- Lista de Clientes ---\n");
    printf("ID / Nombre / NumDispositivos\n");
    printf("-------------------------------------------\n");
    
    if (numClientes == 0 || listaClientes == NULL) {
        printf("No hay clientes registrados actualmente.\n");
    } else {
        for (int i = 0; i < numClientes; i++) {
            printf("%d / %s / %d\n", 
                   listaClientes[i].id, 
                   listaClientes[i].nombre, 
                   listaClientes[i].numDispositivos);
        }
    }
    printf("-------------------------------------------\n\n");

    char linea[10]; 
    int id_seleccionado;
    int encontrado = 0;

    printf("\nIntroduzca el ID del cliente para ver mas informacion (o cualquier letra para volver) > ");
    fflush(stdout);
    
    fgets(linea, 10, stdin);

    if (sscanf(linea, "%d", &id_seleccionado) != 1) {
        return; 
    }



    for (int i = 0; i < numClientes; i++) {
        if (listaClientes[i].id == id_seleccionado) {
            encontrado = 1;
            infoCliente(listaClientes[i]); 
            break; 
        }
    }

    if (!encontrado) {
        printf("\nError: No se ha encontrado ningun cliente con el ID %d.\n", id_seleccionado);
    }
}

void listarDispositivos() {
    printf("\n=======================================================================\n");
    printf("                        LISTA DE DISPOSITIVOS\n");
    printf("=======================================================================\n");
    printf("%-6s | %-20s | %-20s | %-15s\n", "ID", "Nombre", "Cliente", "Version Actual");
    printf("-----------------------------------------------------------------------\n");

    int total = 0;

    for (int i = 0; i < numClientes; i++) {
        
        for (int j = 0; j < listaClientes[i].numDispositivos; j++) {
            
            dispositivo *disp = &listaClientes[i].listaDispositivos[j];

            int version_actual = 0;
            int tiene_config = 0;

            for (int k = 0; k < disp->num_configs; k++) {
                if (disp->configs[k].version > version_actual) {
                    version_actual = disp->configs[k].version;
                    tiene_config = 1; 
                }
            }

            if (tiene_config) {
                printf("%-6d | %-20s | %-20s | v%-14d\n", 
                       disp->id, 
                       disp->nombre, 
                       listaClientes[i].nombre, 
                       version_actual);
            } else {
                printf("%-6d | %-20s | %-20s | %-15s\n", 
                       disp->id, 
                       disp->nombre, 
                       listaClientes[i].nombre, 
                       "Sin conf.");
            }
            total++;
        }
    }

    if (total == 0) {
        printf("No hay ningun dispositivo registrado en el sistema.\n");
    }
    printf("=======================================================================\n");
}

void anadirDispositivo() {
    char entrada_id[10];      
    char nombre_nuevo[10];    
    int idSeleccionado = -1;
    int indiceCliente = -1;
    int nuevoIdDispositivo = 1; 

    
    printf("\n--- LISTA DE CLIENTES DISPONIBLES ---\n");
    for (int i = 0; i < numClientes; i++) {
        printf("ID: %d | Nombre: %s\n", listaClientes[i].id, listaClientes[i].nombre);
    }

   
    printf("\nIntroduzca el ID del cliente al que desea anadir el dispositivo: ");
    fflush(stdout);
    fgets(entrada_id, 10, stdin);
    idSeleccionado = atoi(entrada_id); 

    
    for (int i = 0; i < numClientes; i++) {
        if (listaClientes[i].id == idSeleccionado) {
            indiceCliente = i;
            break;
        }
    }

    
    if (indiceCliente == -1) {
        printf("Error: No se ha encontrado ningun cliente con el ID %d.\n", idSeleccionado);
        return; 
    }
    printf("Introduzca el ID numerico para el nuevo dispositivo: ");
    fflush(stdout);
    fgets(entrada_id, 10, stdin);
    nuevoIdDispositivo = atoi(entrada_id);
    
    printf("Introduzca el nombre del nuevo dispositivo para '%s': ", listaClientes[indiceCliente].nombre);
    fflush(stdout);
    fgets(nombre_nuevo, 100, stdin);
    
  
    nombre_nuevo[strcspn(nombre_nuevo, "\n")] = 0;

 

   
    listaClientes[indiceCliente].numDispositivos++;
    int totalDisp = listaClientes[indiceCliente].numDispositivos;
    
    listaClientes[indiceCliente].listaDispositivos = (dispositivo*) realloc(
        listaClientes[indiceCliente].listaDispositivos, 
        totalDisp * sizeof(dispositivo)
    );

   
    listaClientes[indiceCliente].listaDispositivos[totalDisp - 1].id = nuevoIdDispositivo;
    strcpy(listaClientes[indiceCliente].listaDispositivos[totalDisp - 1].nombre, nombre_nuevo);
    
    
    listaClientes[indiceCliente].listaDispositivos[totalDisp - 1].num_configs = 0;
    listaClientes[indiceCliente].listaDispositivos[totalDisp - 1].configs = NULL;

    printf("\nExito: Dispositivo '%s' (ID: %d) anadido correctamente al cliente '%s'.\n", 
           nombre_nuevo, nuevoIdDispositivo, listaClientes[indiceCliente].nombre);

    // ====================================================================
    // AQUI DEBERIAS HACER EL INSERT EN SQLITE PARA QUE SE GUARDE EN LA BD
    // Ej: "INSERT INTO Dispositivo (ID_DISPOSITIVO, ID_CLIENTE, NOMBRE_DISPOSITIVO) 
    //      VALUES (nuevoIdDispositivo, idSeleccionado, nombre_nuevo);"
    // ====================================================================

    dispositivo nuevo_disp = listaClientes[indiceCliente].listaDispositivos[totalDisp - 1];
    insertDispositivoDB(db, nuevo_disp, idSeleccionado);
}



void crearCliente(){
    cliente cNew;
    char nombre[100];
    int newID = 0;
    
    printf("Introduzca el ID del cliente: ");
    fflush(stdout);


    char str[10];
    fgets(str, 9, stdin);
    sscanf(str, "%d", &newID);

    //fgets(&newID,10,stdin);
    // if(sscanf(newID,"%d", &cNew.id) != 1){
    //     printf("Error\n");
    //     return;
    // }
    
    printf("Introduzca el Nombre del cliente: ");
    fflush(stdout);
    
    fgets(nombre, sizeof(nombre), stdin);

    nombre[strcspn(nombre, "\n")] = 0;

    
    cliente_init(&cNew, newID, nombre);
    

    listaClientes = (cliente*)realloc(listaClientes, sizeof(cliente)*(numClientes+1));
    listaClientes[numClientes] = cNew;
    numClientes++;

    insertClienteBD(db, cNew);
    registrarOperacion(usuario,"Crear cliente");
}

void anadirConfiguracion() {
    char entrada_id[10];
    int idSeleccionado = -1;
    int indiceCliente = -1;
    int indiceDispositivo = -1;

    printf("\nIntroduzca el ID del dispositivo al que desea anadir una configuracion: ");
    fflush(stdout);
    fgets(entrada_id, 10, stdin);
    idSeleccionado = atoi(entrada_id);

    for (int i = 0; i < numClientes; i++) {
        for (int j = 0; j < listaClientes[i].numDispositivos; j++) {
            if (listaClientes[i].listaDispositivos[j].id == idSeleccionado) {
                indiceCliente = i;
                indiceDispositivo = j;
                break;
            }
        }
        if (indiceCliente != -1) break; 
    }

    if (indiceCliente == -1) {
        printf("Error: No se ha encontrado ningun dispositivo con el ID %d.\n", idSeleccionado);
        return;
    }

    cliente *cli = &listaClientes[indiceCliente];
    dispositivo *disp = &cli->listaDispositivos[indiceDispositivo];

    int max_version = 0;
    for (int k = 0; k < disp->num_configs; k++) {
        if (disp->configs[k].version > max_version) {
            max_version = disp->configs[k].version;
        }
    }
    int nueva_version = max_version + 1;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char fecha_actual[256]; 
    sprintf(fecha_actual, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    char ruta_nueva[256]; 
    sprintf(ruta_nueva, "src/confs_cliente/%s_%s_v%d.txt", cli->nombre, disp->nombre, nueva_version);

    FILE *archivo = fopen(ruta_nueva, "w");
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo. Verifica que la carpeta 'src/confs_cliente' exista.\n");
        return; // Salimos de la función para no corromper los datos si falla el archivo
    }

    disp->num_configs++;
    int totalConfigs = disp->num_configs;
    disp->configs = (configuracion*) realloc(disp->configs, totalConfigs * sizeof(configuracion));

    int ultimo = totalConfigs - 1;

    disp->configs[ultimo].version = nueva_version;
    strcpy(disp->configs[ultimo].fecha, fecha_actual);
    strcpy(disp->configs[ultimo].ruta, ruta_nueva);


    printf("\n======================================================\n");
    printf("Exito: Configuracion anadida al dispositivo '%s'.\n", disp->nombre);
    printf("Version : v%d\n", nueva_version);
    printf("Fecha   : %s\n", fecha_actual);
    printf("Ruta    : %s\n", ruta_nueva);
    printf("======================================================\n");

    insertConfiguracionDB(db, disp->configs[ultimo], idSeleccionado);
}

void eliminarClienteUI() {
    char linea[10];
    int id_eliminar;
    int encontrado = 0;

    printf("\n--- Eliminar Cliente ---\n");
    printf("Introduzca el ID del cliente a eliminar: ");
    fflush(stdout);

    fgets(linea, 10, stdin);
    if (sscanf(linea, "%d", &id_eliminar) != 1) {
        printf("Error: ID no valido. Introduce un numero.\n");
        return;
    }

    for (int i = 0; i < numClientes; i++) {
        if (listaClientes[i].id == id_eliminar) {
            encontrado = 1;

            eliminarClienteBD(db, id_eliminar);

            for (int j = i; j < numClientes - 1; j++) {
                listaClientes[j] = listaClientes[j + 1];
            }
            numClientes--;

            if (numClientes > 0) {
                listaClientes = (cliente*)realloc(listaClientes, sizeof(cliente) * numClientes);
            } else {
                free(listaClientes);
                listaClientes = NULL;
            }

            printf("Cliente con ID %d eliminado correctamente.\n\n", id_eliminar);
            break;
        }
    }


    if (!encontrado) {
        printf("No se ha encontrado ningun cliente con el ID %d.\n\n", id_eliminar);
    }
}

void eliminarDispositivo() {
    char entrada_id[10];      
    char confirmacion[10];    
    
    int idSeleccionado = -1;
    int indiceCliente = -1;
    int indiceDispositivo = -1;

    
    printf("\nIntroduzca el ID del dispositivo que desea eliminar: ");
    fflush(stdout);
    fgets(entrada_id, 10, stdin);
    idSeleccionado = atoi(entrada_id);

    for (int i = 0; i < numClientes; i++) {
        for (int j = 0; j < listaClientes[i].numDispositivos; j++) {
            if (listaClientes[i].listaDispositivos[j].id == idSeleccionado) {
                indiceCliente = i;
                indiceDispositivo = j;
                break;
            }
        }
        if (indiceCliente != -1) break; 
    }

    if (indiceCliente == -1) {
        printf("Error: No se ha encontrado ningun dispositivo con el ID %d.\n", idSeleccionado);
        return;
    }

    dispositivo *disp = &listaClientes[indiceCliente].listaDispositivos[indiceDispositivo];
    
    printf("\nSe ha encontrado el dispositivo '%s' (ID: %d) del cliente '%s'.\n", 
           disp->nombre, disp->id, listaClientes[indiceCliente].nombre);
    printf("Esta seguro de que desea eliminarlo permanentemente? (s/n): ");
    fflush(stdout);
    
    fgets(confirmacion, 10, stdin);
    
    if (confirmacion[0] == 's' || confirmacion[0] == 'S') {
        
        if (disp->configs != NULL) {
            free(disp->configs);
        }

        for (int k = indiceDispositivo; k < listaClientes[indiceCliente].numDispositivos - 1; k++) {
            listaClientes[indiceCliente].listaDispositivos[k] = listaClientes[indiceCliente].listaDispositivos[k + 1];
        }

        listaClientes[indiceCliente].numDispositivos--;
        int totalDisp = listaClientes[indiceCliente].numDispositivos;

        if (totalDisp > 0) {
            listaClientes[indiceCliente].listaDispositivos = (dispositivo*) realloc(
                listaClientes[indiceCliente].listaDispositivos, 
                totalDisp * sizeof(dispositivo)
            );
        } else {
            free(listaClientes[indiceCliente].listaDispositivos);
            listaClientes[indiceCliente].listaDispositivos = NULL;
        }

        printf("Exito: Dispositivo eliminado correctamente de la memoria.\n");

        eliminarDispositivoDB(db, idSeleccionado);

    } else {
        printf("Operacion cancelada\n");
    }
}

void MostrarMenuGestionClientes(){
    char linea[10];
    char opcion;
    
    do {
        printf("===========================================\n");
        printf("                GESTION CLIENTES\n");
        printf("===========================================\n\n");

        printf("1. Crear Cliente\n");
        printf("2. Listar Clientes\n");
        printf("3. Eliminar Cliente\n");
        printf("4. Volver\n"); 
        printf("5. Salir\n");

        printf("Seleccione una opcion > ");
        fflush(stdout);
        
    
        fgets(linea, 10, stdin);
        opcion = linea[0];
        
        switch (opcion)
        {
            case '1':
                crearCliente();
                printf("ID: %d\n",listaClientes[numClientes-1].id );
                break;
            case '2':
                ListarClientes(); 
                break; 
            case '3':
                 eliminarClienteUI();
                 break;
            case '4':
                
                break; 
            case '5':
                exit(0);  
                break;
            default:
                if(opcion != '\n') printf("Opcion no valida.\n\n");
                break;
        }
    } while (opcion != '4' && opcion != '5');
  
    return ;

}



int MostrarMenuGestionDispositivos(){
    char opcion;
    char linea[10];
    
    
        do{
            printf("===========================================\n");
            printf("                GESTION DISPOSITIVOS\n");
            printf("===========================================\n\n");

            printf("1. Anadir dispositivos\n");
            printf("2. Listar Dispositivos\n");
            printf("3. Eliminar Dispositivos\n");
            printf("4. Anadir Configuracion\n");
            printf("5. Salir\n");

            printf("Seleccione una opcion > ");

            fflush(stdout);
            char linea[10];
            fgets(linea, 10, stdin);
            opcion = *linea;
            switch (opcion)
            {
                case '1':
                    anadirDispositivo();
                    opcion = 0;
                    break;
                case '2':
                    listarDispositivos();
                    opcion = 0;
                    break; 
                case '3':
                    eliminarDispositivo();
                    break; 
                case '4':
                    anadirConfiguracion();
                    opcion = 0;
                    break;  
            }
        }while (opcion != '5');
        

   
    return 0;
}

int MostrarMenuPrincipal(){
    
    char opcion;
    char linea[10];
    
    do{
        printf("===========================================\n");
        printf("                MENU PRINCIPAL\n");
        printf("===========================================\n\n");

        printf("1. Gestion de clientes\n");
        printf("2. Gestion de dispositivos\n");
        printf("3. Ver registros del sistema\n");
        printf("4. Salir\n");

        printf("Seleccione una opcion > ");

        fflush(stdout);
        
        fgets(linea, 10, stdin);
        
            opcion = *linea;
            switch (opcion)
            {
                case '1':
                    MostrarMenuGestionClientes();
                    break;
                case '2':
                    MostrarMenuGestionDispositivos();
                    opcion = 0;
                    break; 
                case '3':
                    printf("a");
                    break;  
                case '4':
                break;
            }
        }while (opcion != '4');
        printf("Saliendo");
        return 0;
}

void login() {
    
    char contrasena[10];
    int autenticado = 0;

    printf("===========================================\n");
    printf("             INICIO DE SESION\n");
    printf("===========================================\n\n");

    while (!autenticado) {

        printf("Usuario: ");
        fflush(stdout);
        fgets(usuario, sizeof(usuario), stdin);
        usuario[strcspn((const char *)usuario, "\n")] = 0;

        printf("Contrasena: ");
        fflush(stdout);
        fgets(contrasena, sizeof(contrasena), stdin);
        contrasena[strcspn((const char *)contrasena, "\n")] = 0;

        FILE *archivo = fopen("src/usuarios.txt", "r");
        

        if (archivo == NULL) {
            printf("\nError critico: No se pudo abrir el archivo 'usuarios.txt'.\n");
            printf("Asegurese de que el archivo existe en la ruta correcta.\n");
            exit(1); 
        }

        char linea[100];
        char file_user[50];
        char file_pass[50];
        

        while (fgets(linea, sizeof(linea), archivo) != NULL) {
            

            if (sscanf(linea, "%49s %49s", file_user, file_pass) == 2) {
                
                
                if (strcmp(usuario, file_user) == 0 && strcmp(contrasena, file_pass) == 0) {
                    
                    autenticado = 1;
                    break; 
                }
            }
        }

        fclose(archivo);

        if (autenticado) {
            printf("\nAcceso concedido. Bienvenido, %s.\n\n", usuario);
        } else {
            printf("\nError: Usuario o contrasena incorrectos. Vuelva a intentarlo.\n\n");
        }
    }

   
    
    MostrarMenuPrincipal();
    
    
    
}

