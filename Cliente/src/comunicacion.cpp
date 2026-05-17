#include <winsock2.h>
#include <iostream>
#include <string>
#include "../headers/dispositivo.h"
#include "../headers/configuracion.h"
#include "../headers/interfaz.h"
#include "../headers/comunicacion.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

extern Dispositivo* listaDispositivos;
extern int numDispositivos;
extern std::string nombreCliente;
SOCKET sock;


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
    // closesocket(sock);
    // WSACleanup();
    

    return 0;

}

int subirCambios(SOCKET sock) {
    char buffer[512]; // [Longitud: 512 caracteres para mensajes de control]
    
    // 1. Iniciar subida de cambios (sin credenciales, la sesión ya lo sabe)
    char UPLOAD_MSG[20]; // [Longitud: 20 caracteres para el comando UPLOAD_CHANGES]
    strcpy(UPLOAD_MSG, "UPLOAD_CHANGES");
    send(sock, UPLOAD_MSG, strlen(UPLOAD_MSG) + 1, 0);

    // Esperar confirmación del servidor
    recibirMensajeB(sock, buffer, sizeof(buffer)); // Usando tu función habitual de recepción
    if (strcmp(buffer, "OK") != 0) {
        std::cout << "Error: El servidor no acepto la subida de cambios. Pudo perderse la sesion.\n";
        return 0; 
    }

    // 2. Enviar cantidad de dispositivos
    sprintf(buffer, "NUM_DISPOSITIVOS %d", numDispositivos);
    send(sock, buffer, strlen(buffer) + 1, 0);

    // 3. Iterar y enviar todos los datos y archivos
    for (int i = 0; i < numDispositivos; i++) {
        Dispositivo d = listaDispositivos[i];
        sprintf(buffer, "DISPOSITIVO %d %s %d", d.id, d.nombre, d.num_configs);
        std::cout << buffer << "\n";
        send(sock, buffer, strlen(buffer) + 1, 0);

        for (int j = 0; j < d.num_configs; j++) {
            Configuracion c = d.configs[j];
            sprintf(buffer, "CONFIG %d %s %s", c.version, c.ruta, c.fecha);
            send(sock, buffer, strlen(buffer) + 1, 0);

            // Preparar la subida del archivo de texto
            char rutaLocal[256]; // [Longitud: 256 caracteres para ubicar el archivo local]
            sprintf(rutaLocal, "confs/%s", c.ruta);
            
            FILE* f = fopen(rutaLocal, "rb");
            if (f) {
                // Obtener tamaño del archivo
                fseek(f, 0, SEEK_END);
                long fileSize = ftell(f);
                rewind(f);

                // Informar al servidor del tamaño
                sprintf(buffer, "FILE_SIZE %ld", fileSize);
                send(sock, buffer, strlen(buffer) + 1, 0);

                // Subir el contenido en bloques binarios
                char fileBuffer[1024]; // [Longitud: 1024 caracteres para bloque de subida binaria]
                int bytesRead;
                while ((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), f)) > 0) {
                    send(sock, fileBuffer, bytesRead, 0);
                }
                fclose(f);
            } else {
                // Si el archivo no existe o no se puede leer, enviamos tamaño 0
                sprintf(buffer, "FILE_SIZE 0");
                send(sock, buffer, strlen(buffer) + 1, 0);
                std::cout << "Advertencia: No se encontro el archivo local " << rutaLocal << "\n";
            }
        }
    }

    // 4. Finalizar el bloque de subida
    char endMsg[4]; // [Longitud: 4 caracteres para enviar END]
    strcpy(endMsg, "END");
    send(sock, endMsg, strlen(endMsg) + 1, 0);

    std::cout << "Subida de cambios completada con exito.\n";
    return 1;
}

void acabarConexion(SOCKET sock) {
    // 1. Cerrar el socket si está activo
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }

    // 2. Liberar los recursos de Winsock
    WSACleanup();

    std::cout << "Conexion cerrada correctamente. Terminando el programa...\n";

    // 3. Terminar la ejecución del programa con código de éxito (0)
    exit(0);
}