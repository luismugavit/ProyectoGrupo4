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

void anadirDispositivo(){

    char fechaActual[256];
    time_t ahora = time(0);
    struct tm tstruct;
    tstruct = *localtime(&ahora);
    strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y", &tstruct);

    std::cout << listaDispositivos[0].getNombre() << std::endl;
    int nuevoId = 1;
    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].id >= nuevoId) {
            nuevoId = listaDispositivos[i].id + 1;
        }
    }

    Dispositivo* temporal = new Dispositivo[numDispositivos+1];

    for(int i = 0; i < numDispositivos; i++){
        temporal[i] = listaDispositivos[i];
        listaDispositivos[i].configs = nullptr; //DESVINCULAR LISTA DE CONFIGS PARA QUE NO SE BORRE, REFORMAR COPIAS DE LA CLASE DISPOSITIVO
    }
    
    if (temporal == nullptr) {
        std::cout << "Error: No hay memoria suficiente." << std::endl;
        return;
    }
    

    char nombre[100];
    
    std::cout << "Introduce el nombre del dispositivo: ";
    std::cin.ignore(); 
    std::cin.getline(nombre, 100);
    
    Dispositivo dispNuevo = Dispositivo(nuevoId,nombre);


     
    dispNuevo.num_configs = 1;
    dispNuevo.configs = new Configuracion[1];
    dispNuevo.configs[0].version = 1;
    strcpy(dispNuevo.configs[0].fecha, fechaActual); 
    sprintf(dispNuevo.configs[0].ruta, "confs/%s_%s_v%d.txt", nombreCliente.c_str(),dispNuevo.nombre, dispNuevo.configs[0].version);
    std::ofstream archivo(dispNuevo.configs[0].ruta);
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



    delete[] listaDispositivos;

    temporal[numDispositivos] = dispNuevo;

    dispNuevo.configs = nullptr; //DESVINCULAR LISTA DE CONFIGS PARA QUE NO SE BORRE, REFORMAR COPIAS DE LA CLASE DISPOSITIVO

    listaDispositivos = temporal;

    numDispositivos++;
    

    std::cout << "Dispositivo añadido con exito (ID: " << nuevoId << ")" << std::endl;
    
    


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

    Dispositivo* nuevaLista = new Dispositivo[numDispositivos - 1];

    for (int i = 0; i < index; i++) {
        nuevaLista[i] = listaDispositivos[i];
        listaDispositivos[i].configs = nullptr; //DESVINCULAR LISTA DE CONFIGS PARA QUE NO SE BORRE, REFORMAR COPIAS DE LA CLASE DISPOSITIVO
    }

    for (int i = index + 1; i < numDispositivos; i++) {
        nuevaLista[i - 1] = listaDispositivos[i];
        listaDispositivos[i].configs = nullptr; //DESVINCULAR LISTA DE CONFIGS PARA QUE NO SE BORRE, REFORMAR COPIAS DE LA CLASE DISPOSITIVO

    }
    
    delete[] listaDispositivos;
    listaDispositivos = nuevaLista;

    numDispositivos--;
    std::cout << "Dispositivo con ID " << id << " eliminado correctamente.\n";

}