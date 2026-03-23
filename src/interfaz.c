#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MostrarMenuGestionClientes(){
    printf("===========================================\n");
    printf("                GESTION CLIENTES\n");
    printf("===========================================\n\n");

    printf("1. Crear Cliente\n");
    printf("2. Listar Cliente\n");
    printf("3. Eliminar Cliente\n");
    printf("4. Volver\n");
    printf("5. Salir\n");

    printf("Seleccione una opcion > ");

    fflush(stdout);
    char linea[10];
    fgets(linea, 10, stdin);
   

   
    return 0;

}

int MostrarMenuGestionDispositivos(){
    printf("===========================================\n");
    printf("                GESTION DISPOSITIVOS\n");
    printf("===========================================\n\n");

    printf("1. Anyadir dispositivos\n");
    printf("2. Listar Dispositivos\n");
    printf("3. Eliminar Dispositivos\n");
    printf("4. Volver\n");
    printf("5. Salir\n");

    printf("Seleccione una opcion > ");

    fflush(stdout);
    char linea[10];
    fgets(linea, 10, stdin);
   

   
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
                printf("a");
                MostrarMenuGestionClientes();
                break;
            case '2':

                printf("a");
                MostrarMenuGestionDispositivos();
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