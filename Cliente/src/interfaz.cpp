#include "../headers/interfaz.h"
#include <iostream>

void mostrarMenuPrincipal() {
    std::cout << "===========================================\n";
    std::cout << "              MENU PRINCIPAL\n";
    std::cout << "===========================================\n";
    // Resto del menú principal AQUÍ
}

void mostrarMenuGestionDispositivos() {
    std::cout << "===========================================\n";
    std::cout << "           GESTION DISPOSITIVOS\n";
    std::cout << "===========================================\n\n";
    std::cout << "1. Anyadir dispositivos\n";
    std::cout << "2. Listar Dispositivos\n";
    std::cout << "3. Eliminar Dispositivos\n";
    std::cout << "4. Volver\n";
    std::cout << "5. Salir\n";
    std::cout << "Seleccione una opcion > ";
}

void mostrarCabeceraAnyadirDispositivo() {
    std::cout << "\n===========================================\n";
    std::cout << "           ANYADIR DISPOSITIVO\n";
    std::cout << "===========================================\n\n";
}