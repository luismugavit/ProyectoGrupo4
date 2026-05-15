#include <winsock2.h>
#include <iostream>
#include <string>
#include "../headers/dispositivo.h"
#include "../headers/configuracion.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

Dispositivo* listaDispositivos;
int numDispositivos;

// Función para garantizar que se lee un solo mensaje a la vez
int recibirMensaje(SOCKET sock, char* buffer, int max_len) {
    int bytes_leidos = 0;
    char c;
    
    // Leemos byte a byte hasta llenar el buffer o encontrar el terminador \0
    while (bytes_leidos < max_len - 1) {
        int result = recv(sock, &c, 1, 0);
        
        if (result > 0) {
            buffer[bytes_leidos++] = c;
            if (c == '\0') { 
                break; // Llegamos al final del mensaje actual
            }
        } else {
            return result; // Error de red o el servidor cerró la conexión
        }
    }
    buffer[bytes_leidos] = '\0'; // Aseguramos que sea una cadena válida
    return bytes_leidos;
}

const char* obtenerNombreArchivo(const char* ruta) {
    
    const char* ultimoSlash = strrchr(ruta, '/');

    if (ultimoSlash != nullptr) {
        return ultimoSlash + 1; // lo que viene después del '/'
    }

    return ruta; // por si no hay '/'
}

int conectarServidor(){
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[512];

    // Inicializar WinSock
    std::cout << "Inicializando Winsock...\n";
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "Error: " << WSAGetLastError() << "\n";
        return -1;
    }

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Error creando socket\n";
        WSACleanup();
        return -1;
    }

    // Configurar servidor
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Conectar
    std::cout << "Conectando al servidor...\n";
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cout << "Error al conectar\n";
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Conectado al servidor!\n";


    std::string usuario, password;

    std::cout << "Usuario: ";
    std::cin >> usuario;

    std::cout << "Password: ";
    std::cin >> password;


    std::string login_msg = "LOGIN " + usuario + " " + password;
    send(sock, login_msg.c_str(), login_msg.size() + 1, 0);



    recibirMensaje(sock, buffer, sizeof(buffer));

    if (std::string(buffer) != "OK") {
        std::cout << buffer;
        std::cout << "Login incorrecto\n";
        closesocket(sock);
        WSACleanup();
        return 0;
    }

    std::cout << "Login correcto!\n";


    recibirMensaje(sock, buffer, sizeof(buffer));
    sscanf(buffer, "NUM_DISPOSITIVOS %d", &numDispositivos);


    listaDispositivos = new Dispositivo[numDispositivos];


    for (int i = 0; i < numDispositivos; i++) {
        
        recibirMensaje(sock, buffer, sizeof(buffer));
        int id, num_configs;
        char nombre[256];

        std::cout << buffer << "\n";
        sscanf(buffer, "DISPOSITIVO %d %s %d", &id, nombre, &num_configs);

        // Crear dispositivo
        listaDispositivos[i] = Dispositivo(id, nombre);
        

        listaDispositivos[i].num_configs = num_configs;

        listaDispositivos[i].configs = new Configuracion[num_configs];

        for (int j = 0; j < num_configs; j++) {

            recibirMensaje(sock, buffer, sizeof(buffer));
            int version;
            char ruta[256];
            char fecha[256];


            sscanf(buffer, "CONFIG %d %s %s", &version, ruta, fecha);


            const char* nombreArchivo = obtenerNombreArchivo(ruta);
        
 
            listaDispositivos[i].configs[j] = Configuracion(version, nombreArchivo, fecha);
        }
    }


    recibirMensaje(sock, buffer, sizeof(buffer));
    
    // Cerrar
    closesocket(sock);
    WSACleanup();

    return 0;

}

int main() {
    conectarServidor();
    for (int i = 0; i < numDispositivos; i++) {
        std::cout << "Dispositivo " << i + 1 << ": "<< listaDispositivos[i].getNombre() << std::endl;
        std::cout << "Dispositivo " << i + 1 << ": "<< listaDispositivos[i].getConfigs()[0].getRuta() << std::endl;
    }

}

//Prueba