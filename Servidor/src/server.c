#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#include <string.h>
#include "sqlite3.h"
#include "../headers/db.h"
#include "../headers/cliente.h"
#include "../headers/configuracion.h"
#include "../headers/server.h"
#include "../headers/dispositivo.h"
#define PORT 6000
#define SERVER_IP "127.0.0.1"
extern cliente* listaClientes;
extern int numClientes;
extern sqlite3 *db;

int establecerConexion(){

    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len;

    char recvBuff[512];
    char sendBuff[512];

    // Inicializar WinSock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Error al inicializar WinSock\n");
        return 1;
    }

    // Crear socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Error al crear socket\n");
        return 1;
    }

 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error en bind\n");
        return 1;
    }


    listen(server_socket, 5);

    printf("Servidor iniciado en puerto %d...\n", PORT);
    printf("Esperando conexiones...\n");

    while (1) {
        client_len = sizeof(struct sockaddr_in);

        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket == INVALID_SOCKET) {
            printf("Error al aceptar conexion\n");
            continue;
        }

        printf("Cliente conectado!\n");

        
        recv(client_socket, recvBuff, sizeof(recvBuff), 0);
        printf("Mensaje recibido: %s\n", recvBuff);

        char usuario[50], password[50];

        if (strncmp(recvBuff, "LOGIN", 5) == 0) {

            sscanf(recvBuff, "LOGIN %s %s", usuario, password);

            FILE *f = fopen("src/clientes_sesiones.txt", "r");
            int valido = 0;

            if (f != NULL) {
                char file_user[50], file_pass[50];

                while (fscanf(f, "%s %s", file_user, file_pass) != EOF) {
                    if (strcmp(usuario, file_user) == 0 &&
                        strcmp(password, file_pass) == 0) {
                        valido = 1;
                        break;
                    }
                }
                fclose(f);
            }

            if (valido) {
                strcpy(sendBuff, "OK");
                send(client_socket, sendBuff, strlen(sendBuff) + 1, 0);
                printf("Login correcto\n");

                cliente *cliente_encontrado = NULL;

                for (int i = 0; i < numClientes; i++) {
                    if (strcmp(listaClientes[i].nombre, usuario) == 0) {
                        cliente_encontrado = &listaClientes[i];
                        
                        break;
                    }
                }

                if (cliente_encontrado != NULL) {

                    char bufferEnvio[512];

                    sprintf(bufferEnvio, "NUM_DISPOSITIVOS %d",
                            cliente_encontrado->numDispositivos);
                    send(client_socket, bufferEnvio, strlen(bufferEnvio) + 1, 0);

                    for (int i = 0; i < cliente_encontrado->numDispositivos; i++) {

                        dispositivo d = cliente_encontrado->listaDispositivos[i];

                        sprintf(bufferEnvio, "DISPOSITIVO %d %s %d",
                                d.id, d.nombre, d.num_configs);
                        send(client_socket, bufferEnvio, strlen(bufferEnvio) + 1, 0);

                        for (int j = 0; j < d.num_configs; j++) {
                            configuracion c = d.configs[j];

                            sprintf(bufferEnvio, "CONFIG %d %s %s", c.version, c.ruta, c.fecha);
                            printf("%s\n", bufferEnvio);
                            send(client_socket, bufferEnvio, strlen(bufferEnvio) + 1, 0);
                            FILE* file = fopen(c.ruta, "rb");
                            if (file) {
                    
                                fseek(file, 0, SEEK_END);
                                long fileSize = ftell(file);
                                rewind(file);
                                sprintf(bufferEnvio, "FILE_SIZE %ld", fileSize);
                                send(client_socket, bufferEnvio, strlen(bufferEnvio) + 1, 0);

                          
                                char fileBuffer[1024];
                                size_t bytesRead;
                                while ((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), file)) > 0) {
                                    send(client_socket, fileBuffer, bytesRead, 0);
                                }
                                fclose(file);
                            } else {
                                sprintf(bufferEnvio, "FILE_SIZE 0");
                                send(client_socket, bufferEnvio, strlen(bufferEnvio) + 1, 0);
                                printf("Advertencia: No se pudo abrir %s\n", c.ruta);
                            }
                        }
                    }

                    strcpy(bufferEnvio, "END");
                    send(client_socket, bufferEnvio, strlen(bufferEnvio) + 1, 0);
                }
                


            } else {
                strcpy(sendBuff, "FAIL");
                send(client_socket, sendBuff, strlen(sendBuff) + 1, 0);
                printf("Login incorrecto\n");

                closesocket(client_socket);
                continue; // esperar otro cliente
            }
        }

        

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}