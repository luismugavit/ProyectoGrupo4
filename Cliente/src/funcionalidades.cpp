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
void mostrarHistorialDispositivo() {
    int id;
    std::cout << "Introduce el ID del dispositivo para ver su historial: ";
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

    Dispositivo& disp = listaDispositivos[index];

    std::cout << "\n==================================================\n";
    std::cout << "HISTORIAL DE CONFIGURACIONES - " << disp.nombre << " (ID: " << disp.id << ")\n";
    std::cout << "==================================================\n";
    std::cout << "VERSION    FECHA          ARCHIVO\n";
    std::cout << "--------------------------------------------------\n";

    if (disp.num_configs == 0 || disp.configs == nullptr) {
        std::cout << "Este dispositivo no tiene configuraciones registradas.\n";
    } else {
        for (int j = 0; j < disp.num_configs; j++) {
            std::cout << "v" << disp.configs[j].version << "         "
                      << disp.configs[j].fecha << "     "
                      << disp.configs[j].ruta << "\n";
        }
    }
    std::cout << "==================================================\n\n";
}
void verContenidoConfiguracion() {
    int id, version;
    std::cout << "Introduce el ID del dispositivo: ";
    std::cin >> id;
    std::cout << "Introduce la version de configuracion que deseas leer (ej. 1): ";
    std::cin >> version;

    int index = -1;
    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Error: No se encontro el dispositivo.\n";
        return;
    }

    // Buscar si la versión solicitada existe en el dispositivo
    int configIndex = -1;
    for (int j = 0; j < listaDispositivos[index].num_configs; j++) {
        if (listaDispositivos[index].configs[j].version == version) {
            configIndex = j;
            break;
        }
    }

    if (configIndex == -1) {
        std::cout << "Error: La version v" << version << " no existe para este dispositivo.\n";
        return;
    }

    // Construir la ruta al archivo físico
    char rutaNu[256];
    sprintf(rutaNu, "confs/%s", listaDispositivos[index].configs[configIndex].ruta);

    std::ifstream archivo(rutaNu);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo fisico (" << rutaNu << "). Puede que haya sido borrado.\n";
        return;
    }

    std::cout << "\n--- LEYENDO ARCHIVO: " << listaDispositivos[index].configs[configIndex].ruta << " ---\n";
    std::string linea;
    while (std::getline(archivo, linea)) {
        std::cout << linea << "\n";
    }
    std::cout << "--------------------------------------------------\n\n";
    archivo.close();
}
void revertirConfiguracion() {
    int id, versionOrigen;
    std::cout << "Introduce el ID del dispositivo: ";
    std::cin >> id;
    std::cout << "Introduce la version a la que deseas regresar (Rollback): ";
    std::cin >> versionOrigen;

    int index = -1;
    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Error: Dispositivo no encontrado.\n";
        return;
    }

    Dispositivo& disp = listaDispositivos[index];

    // 1. Verificar si la versión destino existe para copiar de ella
    int configOrigenIndex = -1;
    for (int j = 0; j < disp.num_configs; j++) {
        if (disp.configs[j].version == versionOrigen) {
            configOrigenIndex = j;
            break;
        }
    }

    if (configOrigenIndex == -1) {
        std::cout << "Error: La version v" << versionOrigen << " no existe en este dispositivo.\n";
        return;
    }

    // 2. Preparar los datos de la NUEVA versión resultante del rollback
    Configuracion nuevaConf;
    nuevaConf.version = disp.num_configs + 1;
    
    time_t ahora = time(0);
    struct tm tstruct = *localtime(&ahora);
    strftime(nuevaConf.fecha, sizeof(nuevaConf.fecha), "%d/%m/%Y", &tstruct);

    sprintf(nuevaConf.ruta, "%s_%s_v%d.txt", nombreCliente.c_str(), disp.nombre, nuevaConf.version);

    // 3. Leer el archivo antiguo y escribirlo simultáneamente en el nuevo
    char rutaVieja[256];
    char rutaNueva[256];
    sprintf(rutaVieja, "confs/%s", disp.configs[configOrigenIndex].ruta);
    sprintf(rutaNueva, "confs/%s", nuevaConf.ruta);

    std::ifstream archivoViejo(rutaVieja);
    std::ofstream archivoNuevo(rutaNueva);

    if (!archivoViejo.is_open() || !archivoNuevo.is_open()) {
        std::cout << "Error critico al manipular los archivos físicos para el Rollback.\n";
        if (archivoViejo.is_open()) archivoViejo.close();
        return;
    }

    archivoNuevo << "--- ROLLBACK TO VERSION " << versionOrigen << " (GENERATED AS v" << nuevaConf.version << ") ---\n";
    std::string linea;
    while (std::getline(archivoViejo, linea)) {
        if (linea.find("---") == std::string::npos) {
            archivoNuevo << linea << "\n";
        }
    }
    archivoViejo.close();
    archivoNuevo.close();

    int cantidadActual = disp.num_configs;
    Configuracion* tempConfigs = new Configuracion[cantidadActual + 1];

    for (int j = 0; j < cantidadActual; j++) {
        tempConfigs[j] = disp.configs[j];
    }

    tempConfigs[cantidadActual] = nuevaConf;

    if (disp.configs != nullptr) {
        delete[] disp.configs;
    }
    
    disp.configs = tempConfigs;
    disp.num_configs++;

    std::cout << "Rollback completado con exito. El dispositivo " << disp.nombre 
              << " ahora tiene la configuracion de la v" << versionOrigen 
              << " guardada en la nueva v" << nuevaConf.version << ".\n";
}
