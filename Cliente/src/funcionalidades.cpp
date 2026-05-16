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

// --- Lógica de la Funcionalidad solicitada ---
void anyadirDispositivo(Cliente& cliente) {
    mostrarCabeceraAnyadirDispositivo();

    int nuevoId = cliente.calcularNuevoIdDispositivo();
    char nombreDisp[100];

    std::cout << "Introduzca el nombre del nuevo dispositivo: ";
    std::cin >> std::ws; // Limpia posibles saltos de línea residuales
    std::cin.getline(nombreDisp, 100);

    // Creamos el dispositivo usando el idCliente por la PK compuesta
    Dispositivo nuevoDisp(nuevoId, cliente.getId(), nombreDisp);
    
    // Lo añadimos al array dinámico del cliente en memoria
    cliente.agregarDispositivo(nuevoDisp);

    std::cout << "\nExito: Dispositivo '" << nombreDisp << "' (ID: " << nuevoId 
              << ") anadido correctamente al cliente '" << cliente.getNombre() << "'.\n";
}