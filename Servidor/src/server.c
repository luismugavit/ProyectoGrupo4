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

    // Iniciar comunicacion
     WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len;

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
	// Configurar dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

	// Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error en bind\n");
        return 1;
    }
	// Escuchar conexiones

	listen(server_socket, 5);

    printf("Servidor iniciado en puerto %d...\n", PORT);
    printf("Esperando conexiones...\n");

	// Bucle principal
    while (1) {
        client_len = sizeof(struct sockaddr_in);

        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket == INVALID_SOCKET) {
            printf("Error al aceptar conexion\n");
            continue;
        }

        printf("Cliente conectado!\n");

        // Cerramos directamente (solo simulación)
        closesocket(client_socket);
    }

    // Cerrar socket
    closesocket(server_socket);
    WSACleanup();

    
    return 0;

}