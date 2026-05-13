// cliente.cpp
#include <winsock2.h>
#include <iostream>
#include <string>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

int main() {
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

    // Bucle de comunicación
    while (true) {
        std::string comando;
        std::cout << "\nComando (SUMAR / RAIZ / IP / EXIT): ";
        std::cin >> comando;

        // Enviar comando
        send(sock, comando.c_str(), comando.size() + 1, 0);

        if (comando == "SUMAR") {
            std::cout << "Introduce numeros (0 para terminar):\n";

            while (true) {
                std::string num;
                std::cin >> num;

                if (num == "0") {
                    std::string end = "SUMAR-END";
                    send(sock, end.c_str(), end.size() + 1, 0);
                    break;
                }

                send(sock, num.c_str(), num.size() + 1, 0);
            }

            recv(sock, buffer, sizeof(buffer), 0);
            std::cout << "Resultado: " << buffer << "\n";
        }

        else if (comando == "RAIZ") {
            std::string num;
            std::cout << "Numero: ";
            std::cin >> num;

            send(sock, num.c_str(), num.size() + 1, 0);

            std::string end = "RAIZ-END";
            send(sock, end.c_str(), end.size() + 1, 0);

            recv(sock, buffer, sizeof(buffer), 0);
            std::cout << "Resultado: " << buffer << "\n";
        }

        else if (comando == "IP") {
            std::string end = "IP-END";
            send(sock, end.c_str(), end.size() + 1, 0);

            recv(sock, buffer, sizeof(buffer), 0);
            std::cout << "IP del servidor: " << buffer << "\n";
        }

        else if (comando == "EXIT") {
            break;
        }
    }

    // Cerrar
    closesocket(sock);
    WSACleanup();

    return 0;
}