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



void eliminarClienteUI() {
    char linea[10];
    int id_eliminar;
    int encontrado = 0;

    printf("\n--- Eliminar Cliente ---\n");
    printf("Introduzca el ID del cliente a eliminar: ");
    fflush(stdout);

    fgets(linea, 10, stdin);
    if (sscanf(linea, "%d", &id_eliminar) != 1) {
        printf("❌ Error: ID no valido. Introduce un numero.\n");
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
            printf("4. Volver\n");
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
                   
                    opcion = 0;
                    break; 
                case '3':
                    ;
                    break; 
                case '4':
                    
                    break;  
            }
        }while (opcion != '5');
        

   
    return 0;
}

int MostrarMenuPrincipal(){
    
    

    printf("===========================================\n");
    printf("                MENU PRINCIPAL\n");
    printf("===========================================\n\n");

    printf("1. Gestion de clientes\n");
    printf("2. Gestion de dispositivos\n");
    printf("3. Gestion de configuraciones\n");
    printf("4. Ver registros del sistema\n");
    printf("5. Salir\n");

    printf("Seleccione una opcion > ");

    fflush(stdout);
    char linea[10];
    fgets(linea, 10, stdin);
    char opcion;
     do{
        opcion = *linea;
        switch (opcion)
        {
            case '1':
                MostrarMenuGestionClientes();
                opcion = 0;
                break;
            case '2':
                MostrarMenuGestionDispositivos();
                opcion = 0;
                break; 
            case '3':
                printf("a");
                break; 
            case '4':
                printf("a");
                break;  
        }
    }while (opcion != '5');
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

