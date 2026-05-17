#include "../headers/funcionalidades.h"
#include "../headers/interfaz.h"
#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <ctime>

extern Dispositivo* listaDispositivos;
extern int numDispositivos;
extern std::string nombreCliente;
extern Dispositivo* dispositivosNuevos;
extern int numDispositivosNuevos;

// Sistema de Logs
// void registrarLog(const std::string& accion) {
//     std::ofstream file("logs/clienteLogs.txt", std::ios::app);
//     if (file.is_open()) {
//         char fechaActual[256];
//         time_t ahora = time(0);
//         struct tm tstruct = *localtime(&ahora);
//         strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y %H:%M:%S", &tstruct);
//         file << fechaActual << " | " << nombreCliente << " | " << accion << "\n";
//         file.close();
//     }
// }

// // Función auxiliar para registrar cambios en la sesión actual
// void actualizarDispositivosNuevos(int id, const char* nombre, const Configuracion* configOpcional) {
//     for (int i = 0; i < numDispositivosNuevos; i++) {
//         if (dispositivosNuevos[i].id == id) {
//             if (configOpcional != nullptr) dispositivosNuevos[i].agregarConfiguracion(*configOpcional);
//             return;
//         }
//     }
//     // Si no está, lo añadimos
//     Dispositivo d(id, nombre);
//     if (configOpcional != nullptr) d.agregarConfiguracion(*configOpcional);
    
//     Dispositivo* temp = new Dispositivo[numDispositivosNuevos + 1];
//     for (int i = 0; i < numDispositivosNuevos; i++) temp[i] = dispositivosNuevos[i];
//     temp[numDispositivosNuevos] = d;
//     delete[] dispositivosNuevos;
//     dispositivosNuevos = temp;
//     numDispositivosNuevos++;
// }

void anadirDispositivo(){

    char fechaActual[256];
    time_t ahora = time(0);
    struct tm tstruct;
    tstruct = *localtime(&ahora);
    strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y", &tstruct);

    int nuevoId = 1;
    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].id >= nuevoId) {
            nuevoId = listaDispositivos[i].id + 1;
        }
    }

    char nombre[100];
    
    std::cout << "Introduce el nombre del dispositivo: ";
    std::cin.ignore(); 
    std::cin.getline(nombre, 100);
    
    Dispositivo dispNuevo = Dispositivo(nuevoId, nombre);

    Configuracion confNuevo(1, "", fechaActual);
    sprintf(confNuevo.ruta, "confs/%s_%s_v%d.txt", nombreCliente.c_str(), dispNuevo.nombre, 1);
    
    std::ofstream archivo(confNuevo.ruta);
    if (archivo.is_open()) {
        srand(time(NULL) + nuevoId);
        archivo << "--- ROUTER CONFIG FILE ---\n";
        archivo << "Device: " << dispNuevo.nombre << "\n";
        archivo << "Generated: " << fechaActual << "\n";
        archivo << "IPv4: 10.0." << (rand() % 255) << "." << (rand() % 255) << "\n";
        archivo << "Gateway: 10.0.0.1\n";
        archivo << "DNS: 8.8.8.8\n";
        archivo << "Interface: eth" << (rand() % 8) << "\n";
        archivo.close();
    }
    dispNuevo.agregarConfiguracion(confNuevo);
    std::cout << "VVVAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<< std::endl;

    // Redimensionar usando Copy Constructor (¡Sin los hacks de nullptr!)
    Dispositivo* temporal = new Dispositivo[numDispositivos + 1];
    for (int i = 0; i < numDispositivos; i++) temporal[i] = listaDispositivos[i];
    temporal[numDispositivos] = dispNuevo;
    delete[] listaDispositivos;
    listaDispositivos = temporal;
    numDispositivos++;

    // Añadir a pendientes de subir
    //actualizarDispositivosNuevos(nuevoId, nombre, &confNuevo);

    // Log
    //registrarLog("Anyadir dispositivo '" + std::string(nombre) + "' (ID: " + std::to_string(nuevoId) + ")");

    std::cout << "Dispositivo anadido con exito (ID: " << nuevoId << ")" << std::endl;
} 

void listarDispositivos(){
    for(int i = 0; i < numDispositivos; i++){
        if(listaDispositivos[i].num_configs !=0){

         std::cout <<listaDispositivos[i].id << "    " << listaDispositivos[i].nombre <<"       " << listaDispositivos[i].configs[listaDispositivos[i].num_configs-1].getVersion() <<std::endl;
        }else{
            std::cout <<listaDispositivos[i].id << "    " << listaDispositivos[i].nombre <<"       " <<"SIN VERSION" <<std::endl;
        }


    }
}

void eliminarDispositivo(){

    int id;
    std::cout << "Introduce el ID del dispositivo a eliminar: ";
    std::cin >> id;

    int index = -1;

    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Error: No se encontro ningun dispositivo con el ID " << id << ".\n";
        return;
    }
    if (listaDispositivos[index].configs != nullptr) {
        delete[] listaDispositivos[index].configs;
        listaDispositivos[index].configs = nullptr; 
    }


    // if (listaDispositivos[index].configs != nullptr) {
    //     delete[] listaDispositivos[index].configs;
    //     listaDispositivos[index].configs = nullptr; 
    // }

    // Usando copia profunda
    Dispositivo* nuevaLista = new Dispositivo[numDispositivos - 1];
    for (int i = 0, j = 0; i < numDispositivos; i++) {
        if (i != index) nuevaLista[j++] = listaDispositivos[i];
    }
    delete[] listaDispositivos;
    listaDispositivos = nuevaLista;
    numDispositivos--;

    //registrarLog("Eliminar dispositivo (ID: " + std::to_string(id) + ")");
    std::cout << "Dispositivo con ID " << id << " eliminado correctamente.\n";

}