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

void guardarRegistro(const std::string& accion) {
    
    time_t hora = time(0);
    struct tm tstruct = *localtime(&hora);
    char fechaHora[50];
  
    strftime(fechaHora, sizeof(fechaHora), "%d/%m/%Y %H:%M:%S", &tstruct);

 
    std::ofstream archivoLog("logs/registros.txt", std::ios::app);

    if (archivoLog.is_open()) {
        archivoLog << "[" << fechaHora << "] " << accion << "\n";
        archivoLog.close();
    } else {
        std::cerr << "Error: No se pudo abrir el archivo de registros (../logs/registros.txt).\n";
    }
}

void anadirDispositivo(){

    char fechaActual[256];
    time_t ahora = time(0);
    struct tm tstruct = *localtime(&ahora);
    strftime(fechaActual, sizeof(fechaActual), "%d/%m/%Y", &tstruct);


    int nuevoId = 1;
    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].id >= nuevoId)
            nuevoId = listaDispositivos[i].id + 1;
    }


    char nombre[100];
    std::cout << "Introduce el nombre del dispositivo: ";
    std::cin.ignore();
    std::cin.getline(nombre, 100);

    std::cout << "\nTipo de dispositivo:\n";
    std::cout << "  1. Router\n";
    std::cout << "  2. Switch\n";
    std::cout << "  3. Punto de Acceso\n";
    std::cout << "  4. Generico\n";
    std::cout << "Seleccione > ";
    int tipo;
    std::cin >> tipo;

    Dispositivo* dispNuevo = nullptr;

    if (tipo == 1) {
        Router* r = new Router(nuevoId, nombre);
        std::cout << "IP WAN (ej: 81.45.10.1): ";
        std::cin >> r->ip_wan;
        std::cout << "Gateway (ej: 81.45.10.254): ";
        std::cin >> r->gateway;
        std::cout << "Numero de interfaces: ";
        std::cin >> r->num_interfaces;
        dispNuevo = r;
    } else if (tipo == 2) {
        Switch* s = new Switch(nuevoId, nombre);
        std::cout << "Numero de puertos: ";
        std::cin >> s->num_puertos;
        std::cout << "VLAN principal (ej: VLAN10): ";
        std::cin >> s->vlan_principal;
        dispNuevo = s;
    } else if (tipo == 3) {
        PuntoAcceso* p = new PuntoAcceso(nuevoId, nombre);
        std::cout << "SSID de la red: ";
        std::cin >> p->ssid;
        std::cout << "Banda (2.4GHz / 5GHz / Dual): ";
        std::cin >> p->banda;
        dispNuevo = p;
    } else {
        dispNuevo = new Dispositivo(nuevoId, nombre);
    }
    dispNuevo->num_configs = 1;
    dispNuevo->configs = new Configuracion[1];
    dispNuevo->configs[0].version = 1;
    strcpy(dispNuevo->configs[0].fecha, fechaActual);
    sprintf(dispNuevo->configs[0].ruta, "%s_%s_v%d.txt",
            nombreCliente.c_str(), dispNuevo->nombre, 1);

    char rutaNu[256];
    sprintf(rutaNu, "confs/%s", dispNuevo->configs[0].ruta);
    std::ofstream archivo(rutaNu);
    if (archivo.is_open()) {
        srand(time(NULL) + nuevoId);
        archivo << "--- " << dispNuevo->getTipo() << " CONFIG ---\n";
        archivo << "Device: "    << dispNuevo->nombre << "\n";
        archivo << "Type: "      << dispNuevo->getTipo() << "\n";
        archivo << "Generated: " << fechaActual << "\n";
        archivo << "IPv4: 10.0." << (rand()%255) << "." << (rand()%255) << "\n";
        archivo << "Gateway: 10.0.0.1\n";
        archivo.close();
    }

    Dispositivo* temporal = new Dispositivo[numDispositivos + 1];
    for (int i = 0; i < numDispositivos; i++) {
        temporal[i] = listaDispositivos[i];
        listaDispositivos[i].configs = nullptr;
    }
    temporal[numDispositivos] = *dispNuevo;
    dispNuevo->configs = nullptr;
    delete dispNuevo;

    delete[] listaDispositivos;
    listaDispositivos = temporal;
    numDispositivos++;

    std::cout << "Dispositivo añadido con exito (ID: " << nuevoId << ")\n";

    std::string mensajeLog = "Se ha añadido un dispositivo tipo " + std::to_string(tipo) + 
                             " con nombre '" + std::string(nombre) + "' y ID " + std::to_string(nuevoId);
    guardarRegistro(mensajeLog);
}

void listarDispositivos(){
    std::cout << "ID        TIPO          NOMBRE     VERSION\n";
    std::cout << "----  ------------  ------------  -------\n";
    for(int i = 0; i < numDispositivos; i++){
        std::cout << listaDispositivos[i].id << "     "
                  << listaDispositivos[i].getTipo() << "     "
                  << listaDispositivos[i].nombre << "     ";
        if(listaDispositivos[i].num_configs != 0){
            std::cout <<  "     " << listaDispositivos[i].configs[listaDispositivos[i].num_configs-1].getVersion();
        } else {
            std::cout << "SIN VERSION";
        }
        std::cout << "\n";
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
        for (int j = 0; j < listaDispositivos[index].num_configs; j++) {
            char rutaNu[256];
            sprintf(rutaNu, "confs/%s",listaDispositivos[index].configs[j].ruta);
            if (remove(rutaNu) == 0) {
                std::cout << "Archivo eliminado: " << listaDispositivos[index].configs[j].ruta << "\n";
            } else {
                std::cerr << "Aviso: No se pudo borrar el archivo fisico " 
                          << listaDispositivos[index].configs[j].ruta << "\n";
            }
        }
        
        delete[] listaDispositivos[index].configs;
        listaDispositivos[index].configs = nullptr; 
    }



    Dispositivo* nuevaLista = new Dispositivo[numDispositivos - 1];

    for (int i = 0; i < index; i++) {
        nuevaLista[i] = listaDispositivos[i];
        listaDispositivos[i].configs = nullptr; 
    }

    for (int i = index + 1; i < numDispositivos; i++) {
        nuevaLista[i - 1] = listaDispositivos[i];
        listaDispositivos[i].configs = nullptr; 

    }
    
    delete[] listaDispositivos;
    listaDispositivos = nuevaLista;

    numDispositivos--;
    std::cout << "Dispositivo con ID " << id << " eliminado correctamente.\n";
    guardarRegistro("Se ha eliminado el dispositivo con ID " + std::to_string(id));

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

    std::string mensajeLog = "Se ha añadido la configuracion v" + std::to_string(nuevaConf.version) + 
                             " al dispositivo '" + listaDispositivos[index].nombre + "'";
    guardarRegistro(mensajeLog);


}