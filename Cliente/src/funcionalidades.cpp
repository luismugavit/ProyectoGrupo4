#include "../headers/funcionalidades.h"
#include "../headers/interfaz.h"
#include <iostream>
#include <cstring>

// --- Implementación de Cliente ---
Cliente::Cliente() : id(0), listaDispositivos(nullptr), numDispositivos(0) {
    nombre[0] = '\0';
}

Cliente::Cliente(int id, const char* nombre) : id(id), listaDispositivos(nullptr), numDispositivos(0) {
    strncpy(this->nombre, nombre, 99);
    this->nombre[99] = '\0';
}

Cliente::Cliente(const Cliente& otro) : id(otro.id), numDispositivos(otro.numDispositivos) {
    strcpy(nombre, otro.nombre);
    if (numDispositivos > 0) {
        listaDispositivos = new Dispositivo[numDispositivos];
        for (int i = 0; i < numDispositivos; i++) {
            listaDispositivos[i] = otro.listaDispositivos[i];
        }
    } else {
        listaDispositivos = nullptr;
    }
}

Cliente& Cliente::operator=(const Cliente& otro) {
    if (this != &otro) {
        delete[] listaDispositivos;
        id = otro.id;
        strcpy(nombre, otro.nombre);
        numDispositivos = otro.numDispositivos;
        
        if (numDispositivos > 0) {
            listaDispositivos = new Dispositivo[numDispositivos];
            for (int i = 0; i < numDispositivos; i++) {
                listaDispositivos[i] = otro.listaDispositivos[i];
            }
        } else {
            listaDispositivos = nullptr;
        }
    }
    return *this;
}

Cliente::~Cliente() {
    delete[] listaDispositivos;
}

int Cliente::getId() const { return id; }
const char* Cliente::getNombre() const { return nombre; }
int Cliente::getNumDispositivos() const { return numDispositivos; }

void Cliente::agregarDispositivo(const Dispositivo& d) {
    Dispositivo* nuevo = new Dispositivo[numDispositivos + 1];
    for (int i = 0; i < numDispositivos; i++) {
        nuevo[i] = listaDispositivos[i];
    }
    nuevo[numDispositivos] = d;
    
    delete[] listaDispositivos;
    listaDispositivos = nuevo;
    numDispositivos++;
}

int Cliente::calcularNuevoIdDispositivo() const {
    if (numDispositivos == 0) {
        return 1;
    }
    int maxId = 0;
    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].getId() > maxId) {
            maxId = listaDispositivos[i].getId();
        }
    }
    return maxId + 1;
}

int Cliente::buscarDispositivoPorId(int id) const {
    for (int i = 0; i < numDispositivos; i++) {
        if (listaDispositivos[i].getId() == id) {
            return i;
        }
    }
    return -1; // Retorna -1 si no lo encuentra
}

bool Cliente::removerDispositivo(int id) {
    int index = buscarDispositivoPorId(id);
    if (index == -1) return false;

    // Si solo hay un dispositivo, liberamos la memoria y dejamos el array vacío
    if (numDispositivos == 1) {
        delete[] listaDispositivos;
        listaDispositivos = nullptr;
        numDispositivos = 0;
        return true;
    }

    // Si hay más de uno, creamos un nuevo array con un tamaño menor
    Dispositivo* nuevo = new Dispositivo[numDispositivos - 1];
    for (int i = 0, j = 0; i < numDispositivos; i++) {
        if (i != index) {
            nuevo[j++] = listaDispositivos[i];
        }
    }
    
    delete[] listaDispositivos;
    listaDispositivos = nuevo;
    numDispositivos--;
    return true;
}

// --- Funcionalidad 1: Añadir Dispositivo ---
void anyadirDispositivo(Cliente& cliente) {
    mostrarCabeceraAnyadirDispositivo();

    int nuevoId = cliente.calcularNuevoIdDispositivo();
    char nombreDisp[100];

    std::cout << "Introduzca el nombre del nuevo dispositivo: ";
    std::cin >> std::ws;
    std::cin.getline(nombreDisp, 100);

    Dispositivo nuevoDisp(nuevoId, cliente.getId(), nombreDisp);
    cliente.agregarDispositivo(nuevoDisp);

    std::cout << "\nExito: Dispositivo '" << nombreDisp << "' (ID: " << nuevoId 
              << ") anadido correctamente al cliente '" << cliente.getNombre() << "'.\n";
}

// --- Funcionalidad 2: Listar Dispositivos ---
void listarDispositivos(const Cliente& cliente) {
    std::cout << "\n===========================================\n";
    std::cout << "           LISTAR DISPOSITIVOS\n";
    std::cout << "===========================================\n\n";

    if (cliente.getNumDispositivos() == 0) {
        std::cout << "No hay dispositivos registrados.\n";
        return;
    }

    std::cout << "ID | Nombre | Version Actual\n";
    for (int i = 0; i < cliente.getNumDispositivos(); i++) {
        const Dispositivo& d = cliente.listaDispositivos[i];
        std::cout << d.getId() << " | " << d.getNombre() << " | ";
        
        if (d.getNumConfigs() == 0) {
            std::cout << "Sin config\n";
        } else {
            // Obtenemos la última configuración para mostrar la versión actual
            int ultimaVersion = d.getConfigs()[d.getNumConfigs() - 1].getVersion();
            std::cout << "v" << ultimaVersion << "\n";
        }
    }
}

// --- Funcionalidad 3: Eliminar Dispositivo ---
void eliminarDispositivo(Cliente& cliente) {
    std::cout << "\n===========================================\n";
    std::cout << "          ELIMINAR DISPOSITIVO\n";
    std::cout << "===========================================\n\n";

    if (cliente.getNumDispositivos() == 0) {
        std::cout << "No hay dispositivos registrados para eliminar.\n";
        return;
    }

    int idEliminar;
    std::cout << "Introduzca el ID del dispositivo a eliminar: ";
    std::cin >> idEliminar;

    int index = cliente.buscarDispositivoPorId(idEliminar);
    if (index == -1) {
        std::cout << "Error: No se encontro ningun dispositivo con ID " << idEliminar << ".\n";
        return;
    }

    std::cout << "Si continua el dispositivo se eliminara [s/n]: ";
    char confirmacion;
    std::cin >> confirmacion;

    if (confirmacion == 's' || confirmacion == 'S') {
        cliente.removerDispositivo(idEliminar);
        std::cout << "Dispositivo con ID " << idEliminar << " eliminado correctamente.\n";
    } else {
        std::cout << "Operacion cancelada.\n";
    }
}