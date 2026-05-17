#include "../headers/funcionalidades.h"
#include "../headers/interfaz.h"
#include "../headers/configuracion.h"
#include "../headers/dispositivo.h"
#include <iostream>
#include <cstring>
#include <cstdio>
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
    sprintf(dispNuevo.configs[0].ruta, "%s_%s_v%d.txt", nombreCliente.c_str(),dispNuevo.nombre, dispNuevo.configs[0].version);
    char rutaNu[256];
    sprintf(rutaNu, "confs/%s",dispNuevo.configs[0].ruta);
    //std::cout << rutaNu << std::endl;
    std::ofstream archivo(rutaNu);
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
    }else{
            std::cout << "NO ENCONTRAO" << std::endl;

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

    // --- NUEVA LÓGICA: ELIMINAR ARCHIVOS FÍSICOS ---
    if (listaDispositivos[index].configs != nullptr) {
        for (int j = 0; j < listaDispositivos[index].num_configs; j++) {
            // Intentamos borrar el archivo del disco usando la ruta guardada
            char rutaNu[256];
            sprintf(rutaNu, "confs/%s",listaDispositivos[index].configs[j].ruta);
            if (remove(rutaNu) == 0) {
                std::cout << "Archivo eliminado: " << listaDispositivos[index].configs[j].ruta << "\n";
            } else {
                // Si el archivo no existe o está bloqueado, no detenemos el programa
                std::cerr << "Aviso: No se pudo borrar el archivo fisico " 
                          << listaDispositivos[index].configs[j].ruta << "\n";
            }
        }
        
        // Ahora que los archivos no están, liberamos la memoria del array de configs
        delete[] listaDispositivos[index].configs;
        listaDispositivos[index].configs = nullptr; 
    }


    // if (listaDispositivos[index].configs != nullptr) {
    //     delete[] listaDispositivos[index].configs;
    //     listaDispositivos[index].configs = nullptr; 
    // }

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

void anadirConfiguracion(){

    listarDispositivos();

    int id;
    std::cout << "Introduce el ID del dispositivo al que quiere anadir configuracion: ";
    std::cin >> id;

    int index = -1;

    for(int i = 0; i < numDispositivos; i ++){

        if(listaDispositivos[i].id == id){
            index = i;
        }

    }

    if (index == -1) {
        std::cout << "Error: No se encontro ningun dispositivo con el ID " << id << ".\n";
        return;
    }

    Configuracion nuevaConf;
    nuevaConf.version = listaDispositivos[index].num_configs + 1;
    time_t ahora = time(0);
    struct tm tstruct = *localtime(&ahora);
    strftime(nuevaConf.fecha, sizeof(nuevaConf.fecha), "%d/%m/%Y", &tstruct);

    sprintf(nuevaConf.ruta, "%s_%s_v%d.txt", 
            nombreCliente.c_str(), 
            listaDispositivos[index].nombre, nuevaConf.version);
    
    int cantidadActual = listaDispositivos[index].num_configs;
    Configuracion* tempConfigs = new Configuracion[cantidadActual + 1];

    for (int j = 0; j < cantidadActual; j++) {
        tempConfigs[j] = listaDispositivos[index].configs[j];
    }

    tempConfigs[cantidadActual] = nuevaConf;
    if (listaDispositivos[index].configs != nullptr) {
        delete[] listaDispositivos[index].configs;
    }
    
    listaDispositivos[index].configs = tempConfigs;
    listaDispositivos[index].num_configs++;
    char rutaNu[256];
    sprintf(rutaNu, "confs/%s",nuevaConf.ruta);
    std::ofstream archivo(rutaNu);
    if (archivo.is_open()) {
        archivo << "--- CONFIGURACION VERSION " << nuevaConf.version << " ---\n";
        archivo << "Fecha: " << nuevaConf.fecha << "\n";
        archivo << "IP: 10.0.0." << (rand() % 254) << "\n";
        archivo.close();
    }

    std::cout << "Configuracion v" << nuevaConf.version << " añadida con exito al dispositivo " 
              << listaDispositivos[index].nombre << std::endl;


}