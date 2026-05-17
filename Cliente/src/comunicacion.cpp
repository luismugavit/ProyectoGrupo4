#include <winsock2.h>
#include <iostream>
#include <string>
#include "../headers/dispositivo.h"
#include "../headers/configuracion.h"
#include "../headers/interfaz.h"
#include "../headers/comunicacion.h"

#include <fstream>
#include "../headers/funcionalidades.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

extern Dispositivo* listaDispositivos;
extern int numDispositivos;
extern std::string nombreCliente;

extern Dispositivo* dispositivosNuevos;
extern int numDispositivosNuevos;
extern std::string loginUser;
extern std::string loginPass;

// Función para garantizar que se lee un solo mensaje a la vez
int recibirMensajeB(SOCKET sock, char* buffer, int max_len) {
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

const char* obtenerNombreArchivoB(const char* ruta) {
    
    const char* ultimoSlash = strrchr(ruta, '/');

    if (ultimoSlash != nullptr) {
        return ultimoSlash + 1; 
    }

    return ruta;
}

int conectarServidorB(){
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

    // Guardar las credenciales
    loginUser = usuario;
    loginPass = password;


    std::string login_msg = "LOGIN " + usuario + " " + password;
    send(sock, login_msg.c_str(), login_msg.size() + 1, 0);



    recibirMensajeB(sock, buffer, sizeof(buffer));

    if (std::string(buffer) != "OK") {
        std::cout << buffer;
        std::cout << "Login incorrecto\n";
        closesocket(sock);
        WSACleanup();
        return 0;
    }

    std::cout << "Login correcto!\n";


    recibirMensajeB(sock, buffer, sizeof(buffer));
    sscanf(buffer, "NUM_DISPOSITIVOS %d", &numDispositivos);

    
    listaDispositivos = new Dispositivo[numDispositivos];
   

    for (int i = 0; i < numDispositivos; i++) {
        
        recibirMensajeB(sock, buffer, sizeof(buffer));
        int id, num_configs;
        char nombre[256];

        std::cout << buffer << "\n";
        sscanf(buffer, "DISPOSITIVO %d %s %d", &id, nombre, &num_configs);

        // Crear dispositivo
        listaDispositivos[i] = Dispositivo(id, nombre);
        std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaa\n";
        //ARREGLAR
        Dispositivo dispTemporal(id, nombre);
        dispTemporal.configs = nullptr;
        dispTemporal.num_configs = 0;
        listaDispositivos[i] = dispTemporal;
        dispTemporal.configs = nullptr;

        listaDispositivos[i].num_configs = num_configs;

        listaDispositivos[i].configs = new Configuracion[num_configs];
        for (int j = 0; j < num_configs; j++) {
            recibirMensajeB(sock, buffer, sizeof(buffer));
            int version;
            char ruta[256]; 
            char fecha[50]; 

            
            sscanf(buffer, "CONFIG %d %s %s", &version, ruta, fecha);
            const char* nombreArchivo = obtenerNombreArchivoB(ruta);
                

            listaDispositivos[i].configs[j] = Configuracion(version, nombreArchivo, fecha);
            //Descargar el archivo
            recibirMensajeB(sock, buffer, sizeof(buffer));
            long fileSize = 0;
            sscanf(buffer, "FILE_SIZE %ld", &fileSize);

            if (fileSize > 0) {
                char rutaDestino[512];
                sprintf(rutaDestino, "confs/%s", nombreArchivo);

                FILE* file = fopen(rutaDestino, "wb");
                if (file) {
                    long bytesRestantes = fileSize;
                    char fileBuffer[1024]; 
                    
                    while (bytesRestantes > 0) {
                        
                        int bytesALeer = (bytesRestantes < sizeof(fileBuffer)) ? bytesRestantes : sizeof(fileBuffer);
                        int bytesLeidos = recv(sock, fileBuffer, bytesALeer, 0);
                        
                        if (bytesLeidos > 0) {
                            fwrite(fileBuffer, 1, bytesLeidos, file);
                            bytesRestantes -= bytesLeidos;
                        } else {
                            std::cout << "Error en la descarga del archivo: " << nombreArchivo << "\n";
                            break; 
                        }
                    }
                    fclose(file);
                    std::cout << "Descargado: " << rutaDestino << " (" << fileSize << " bytes)\n";
                } else {
                    std::cout << "Error: No se pudo crear " << rutaDestino << " (Comprueba que la carpeta existe)\n";
                    char dumpBuffer[1024]; 
                    long remaining = fileSize;
                    while(remaining > 0){
                        int readSize = (remaining < sizeof(dumpBuffer)) ? remaining : sizeof(dumpBuffer);
                        remaining -= recv(sock, dumpBuffer, readSize, 0);
                    }
                }
            } else {
                std::cout << "El archivo " << nombreArchivo << " está vacío o no existe en el servidor.\n";
            }
        }
                

    }


    recibirMensajeB(sock, buffer, sizeof(buffer));
    closesocket(sock);
    WSACleanup();
    

    return 0;

}

// void subirAlServidor() {
//     if (numDispositivosNuevos == 0) { std::cout << "No hay cambios nuevos que subir.\n"; return; }

//     WSADATA wsaData; SOCKET sock; struct sockaddr_in server; char buffer[512];
//     WSAStartup(MAKEWORD(2,2), &wsaData); sock = socket(AF_INET, SOCK_STREAM, 0);
//     server.sin_family = AF_INET; server.sin_port = htons(SERVER_PORT); server.sin_addr.s_addr = inet_addr(SERVER_IP);
//     connect(sock, (struct sockaddr*)&server, sizeof(server));

//     std::string login_msg = "LOGIN " + loginUser + " " + loginPass;
//     send(sock, login_msg.c_str(), login_msg.size() + 1, 0);
//     recibirMensajeB(sock, buffer, sizeof(buffer));

//     // Drenar datos viejos que envia el server
//     recibirMensajeB(sock, buffer, sizeof(buffer)); int numD; sscanf(buffer, "NUM_DISPOSITIVOS %d", &numD);
//     for (int i = 0; i < numD; i++) {
//         recibirMensajeB(sock, buffer, sizeof(buffer)); int id, nc; char nm[256]; sscanf(buffer, "DISPOSITIVO %d %s %d", &id, nm, &nc);
//         for (int j = 0; j < nc; j++) {
//             recibirMensajeB(sock, buffer, sizeof(buffer)); recibirMensajeB(sock, buffer, sizeof(buffer));
//             long fileSize = 0; sscanf(buffer, "FILE_SIZE %ld", &fileSize);
//             long restantes = fileSize; char fileBuf[1024];
//             while (restantes > 0) {
//                 int aLeer = (restantes < sizeof(fileBuf)) ? restantes : sizeof(fileBuf);
//                 int leidos = recv(sock, fileBuf, aLeer, 0);
//                 if (leidos > 0) restantes -= leidos; else break;
//             }
//         }
//     }
//     recibirMensajeB(sock, buffer, sizeof(buffer)); // END

//     // PROTOCOLO DE SUBIDA
//     std::string uploadStart = "UPLOAD_START"; send(sock, uploadStart.c_str(), uploadStart.size() + 1, 0);

//     for (int i = 0; i < numDispositivosNuevos; i++) {
//         Dispositivo& d = dispositivosNuevos[i]; char msg[256];
//         sprintf(msg, "DISPOSITIVO %d %s %d", d.id, d.nombre, d.num_configs);
//         send(sock, msg, strlen(msg) + 1, 0);

//         for (int j = 0; j < d.num_configs; j++) {
//             Configuracion& c = d.configs[j];
//             sprintf(msg, "CONFIG %d %s %s", c.version, c.ruta, c.fecha); send(sock, msg, strlen(msg) + 1, 0);
            
//             std::ifstream file(c.ruta, std::ios::binary | std::ios::ate);
//             if (file.is_open()) {
//                 long size = file.tellg(); file.seekg(0, std::ios::beg);
//                 sprintf(msg, "FILE_SIZE %ld", size); send(sock, msg, strlen(msg) + 1, 0);
//                 char* fileBuffer = new char[size];
//                 if (file.read(fileBuffer, size)) send(sock, fileBuffer, size, 0);
//                 delete[] fileBuffer; file.close();
//             } else {
//                 sprintf(msg, "FILE_SIZE 0"); send(sock, msg, strlen(msg) + 1, 0);
//             }
//         }
//     }

//     std::string uploadEnd = "UPLOAD_END"; send(sock, uploadEnd.c_str(), uploadEnd.size() + 1, 0);
//     recibirMensajeB(sock, buffer, sizeof(buffer));
    
//     if (std::string(buffer) == "UPLOAD_OK") {
//        // registrarLog("Subir cambios al servidor (" + std::to_string(numDispositivosNuevos) + " dispositivos)");
//         std::cout << "Cambios subidos correctamente al servidor.\n";
//         delete[] dispositivosNuevos; dispositivosNuevos = nullptr; numDispositivosNuevos = 0;
//     }
//     closesocket(sock); WSACleanup();
// }