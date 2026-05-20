#include "../headers/interfaz.h"
#include "../headers/comunicacion.h"
#include "../headers/funcionalidades.h"
#include <iostream>
#include <winsock2.h>

extern Dispositivo* listaDispositivos;
extern int numDispositivos;
extern SOCKET sock;

void mostrarMenuPrincipal() {
    std::cout << "===========================================\n";
    std::cout << "              MENU PRINCIPAL\n";
    std::cout << "===========================================\n";
    std::cout << "1. Gestionar Dispositivos\n";
    std::cout << "2. Guardar cambios en servidor\n";
    std::cout << "3. Salir\n";
    std::cout << "Seleccione una opcion > ";
}

void mostrarMenuGestionDispositivos() {
    std::cout << "===========================================\n";
    std::cout << "           GESTION DISPOSITIVOS\n";
    std::cout << "===========================================\n\n";
    std::cout << "1. Anyadir dispositivos\n";
    std::cout << "2. Listar Dispositivos\n";
    std::cout << "3. Eliminar Dispositivos\n";
    std::cout << "4. Anadir Configuracion\n";
    std::cout << "5. Ver Historial de Configuraciones\n";
    std::cout << "6. Ver Contenido de una Configuracion\n";
    std::cout << "7. Revertir a Version Anterior (Rollback)\n"; 
    std::cout << "8. Volver\n";
    std::cout << "9. Salir\n";
    std::cout << "Seleccione una opcion > ";
}

void ejecutarMenuPrincipal(){
    int opcion;
    bool volver = false;
    do {
        mostrarMenuPrincipal();
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                ejecutarMenuGestionDispositivos();
                break;

            case 2:
                subirCambios(sock);
                break;

            case 3:
                std::cout << "Saliendo del programa...\n";
                exit(0); 
                break;

            default:
                std::cout << "Opcion no valida. Intentalo de nuevo.\n";
                break;
        }

        std::cout << "\n"; 

    } while (!volver);
}

void ejecutarMenuGestionDispositivos() {
    int opcion;
    bool volver = false;

    do {
        mostrarMenuGestionDispositivos();
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                anadirDispositivo(); 
                break;

            case 2:
                std::cout << "===========================================\n";
                std::cout << "           LISTA DISPOSITIVOS\n";
                std::cout << "===========================================\n\n";
                std::cout << "ID    NOMBRE       VERSION\n";
                listarDispositivos();
                break;

            case 3:
                eliminarDispositivo();
                break;

            case 4:
                anadirConfiguracion();
                break;

            case 5:
                mostrarHistorialDispositivo();
                break;

            case 6:
                verContenidoConfiguracion();
                break;

            case 7: 
                revertirConfiguracion();
                break;

            case 8: 
                std::cout << "Volviendo al menu principal...\n";
                volver = true;
                break;

            case 9: 
                std::cout << "Saliendo del programa...\n";
                exit(0);
                break;

            default:
                std::cout << "Opcion no valida. Intentalo de nuevo.\n";
                break;
        }

        std::cout << "\n"; 

    } while (!volver);
}

void mostrarCabeceraAnyadirDispositivo() {
    std::cout << "\n===========================================\n";
    std::cout << "           ANYADIR DISPOSITIVO\n";
    std::cout << "===========================================\n\n";
}