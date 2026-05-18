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

#include <stdio.h>
#include <stdbool.h>

/**
 * Compara la lista guardada en memoria (vieja) con la recibida por el socket (nueva).
 * Identifica qué IDs han desaparecido para poder borrarlos de la base de datos.
 */
void detectarDispositivosEliminados(dispositivo *listaVieja, int numVieja, 
                                   dispositivo *listaNueva, int numNueva,
                                   int id_cliente, sqlite3 *db) {
    
    printf("--- Iniciando comparación de dispositivos para cliente %d ---\n", id_cliente);

    for (int i = 0; i < numVieja; i++) {
        bool encontrado = false;
        int idBuscado = listaVieja[i].id;

        // Buscamos el ID viejo en la lista nueva
        for (int j = 0; j < numNueva; j++) {
            if (listaNueva[j].id == idBuscado) {
                encontrado = true;
                break;
            }
        }

        // Si no se encontró en la lista nueva, es que ha sido eliminado
        if (!encontrado) {
            printf("[ALERTA] El dispositivo ID: %d (%s) ya no existe en el cliente. Borrando...\n", 
                    idBuscado, listaVieja[i].nombre);
            
            for (int k = 0; k < listaVieja[i].num_configs; k++) {
                const char* rutaArchivo = listaVieja[i].configs[k].ruta;
                
                if (remove(rutaArchivo) == 0) {
                    printf("  -> Archivo físico eliminado: %s\n", rutaArchivo);
                } else {
                    // Usamos perror para ver el motivo exacto (ej: archivo no existe)
                    perror("  -> Error al eliminar archivo físico");
                }
            }


            // Llamamos a tu función de base de datos
            int res = eliminarDispositivoDB(db, listaVieja[i], id_cliente);
            
            if (res == SQLITE_OK) {
                printf("Dispositivo %d eliminado con éxito de la BD.\n", idBuscado);
            }
        }
    }
    printf("--- Fin de la sincronización de borrado ---\n");
}

// Función para evitar concatenación TCP en el lado del servidor
int recibirMensajeServidor(SOCKET sock, char* buffer, int max_len) {
    int bytes_leidos = 0;
    char c; 
    
    while (bytes_leidos < max_len - 1) {
        int result = recv(sock, &c, 1, 0);
        if (result > 0) {
            buffer[bytes_leidos++] = c;
            if (c == '\0') break;
        } else {
            return result;
        }
    }
    buffer[bytes_leidos] = '\0';
    return bytes_leidos;
}

int establecerConexion(){

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

        int clienteActivo = 1;
        cliente *cliente_encontrado = NULL; // Guarda la sesión actual
        
        // Bucle de sesión persistente
        while (clienteActivo) {
            char recvBuff[512]; // [Longitud: 512 caracteres para recibir comandos del cliente]
            int bytesRecibidos = recv(client_socket, recvBuff, sizeof(recvBuff), 0);

            if (bytesRecibidos <= 0) {
                printf("Cliente desconectado de forma limpia o por error de red.\n");
                clienteActivo = 0;
                break;
            }

            printf("Mensaje recibido: %s\n", recvBuff);

            if (strncmp(recvBuff, "LOGIN", 5) == 0) {
                char usuario[50]; // [Longitud: 50 caracteres para el nombre de usuario]
                char password[50]; // [Longitud: 50 caracteres para la contraseña]
                sscanf(recvBuff, "LOGIN %s %s", usuario, password);

                FILE *f = fopen("src/clientes_sesiones.txt", "r");
                int valido = 0;

                if (f != NULL) {
                    char file_user[50]; // [Longitud: 50 caracteres para usuario del archivo]
                    char file_pass[50]; // [Longitud: 50 caracteres para contraseña del archivo]

                    while (fscanf(f, "%s %s", file_user, file_pass) != EOF) {
                        if (strcmp(usuario, file_user) == 0 &&
                            strcmp(password, file_pass) == 0) {
                            valido = 1;
                            break;
                        }
                    }
                    fclose(f);
                }

                char sendBuff[512]; // [Longitud: 512 caracteres para mensajes de estado]

                if (valido) {
                    strcpy(sendBuff, "OK");
                    send(client_socket, sendBuff, strlen(sendBuff) + 1, 0);
                    printf("Login correcto\n");

                    // Vincular la sesión
                    for (int i = 0; i < numClientes; i++) {
                        if (strcmp(listaClientes[i].nombre, usuario) == 0) {
                            cliente_encontrado = &listaClientes[i];
                            break;
                        }
                    }

                    if (cliente_encontrado != NULL) {
                        char bufferEnvio[512]; // [Longitud: 512 caracteres para metadatos salientes]

                        sprintf(bufferEnvio, "NUM_DISPOSITIVOS %d", cliente_encontrado->numDispositivos);
                        send(client_socket, bufferEnvio, strlen(bufferEnvio) + 1, 0);

                        for (int i = 0; i < cliente_encontrado->numDispositivos; i++) {
                            dispositivo d = cliente_encontrado->listaDispositivos[i];

                            sprintf(bufferEnvio, "DISPOSITIVO %d %s %d", d.id, d.nombre, d.num_configs);
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

                                    char fileBuffer[1024]; // [Longitud: 1024 caracteres para bloque de envio de fichero]
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
                    clienteActivo = 0; // Desconectar a este cliente si falla el login
                }
            } 
            else if (strncmp(recvBuff, "UPLOAD_CHANGES", 14) == 0) {
                
                // Comprobar que hay sesión activa
                if (cliente_encontrado == NULL) {
                    char sendBuff[10]; // [Longitud: 10 caracteres para rechazar]
                    strcpy(sendBuff, "FAIL");
                    send(client_socket, sendBuff, strlen(sendBuff) + 1, 0);
                    printf("Intento de subida sin sesion. Desconectando.\n");
                    clienteActivo = 0;
                    break;
                }

                char sendBuff[10]; // [Longitud: 10 caracteres para confirmar subida]
                strcpy(sendBuff, "OK");
                send(client_socket, sendBuff, strlen(sendBuff) + 1, 0);

                char buffer[512]; // [Longitud: 512 caracteres para metadatos entrantes]
                recibirMensajeServidor(client_socket, buffer, sizeof(buffer));
                
                int numNuevos;
                sscanf(buffer, "NUM_DISPOSITIVOS %d", &numNuevos);

                dispositivo* nuevosDispositivos = (dispositivo*)malloc(numNuevos * sizeof(dispositivo));

                for (int i = 0; i < numNuevos; i++) {
                    recibirMensajeServidor(client_socket, buffer, sizeof(buffer));
    
                    // 1. Inicializar variables para evitar arrastrar basura o valores de la vuelta anterior
                    int id = -1;
                    int num_configs = -1;
                    char nombre[100] = ""; // [Longitud: 100 caracteres para el nombre del dispositivo]
                    
                    printf("Buffer recibido: '%s'\n", buffer);
                    
                    // 2. Guardar el resultado de sscanf (debe devolver 3 si lee todo correctamente)
                    int leidos = sscanf(buffer, "DISPOSITIVO %d %s %d", &id, nombre, &num_configs);
                    
                    // 3. Comprobación de errores
                    if (leidos != 3) {
                        printf("ERROR LECTURA: sscanf solo leyo %d de 3 elementos.\n", leidos);
                        printf("Valores actuales -> ID: %d, Nombre: '%s', Configs: %d\n", id, nombre, num_configs);
                        
                        // Si 'nombre' tiene la primera palabra pero 'configs' es -1, el problema son los espacios en el nombre.
                    } else {
                        printf("Lectura exitosa -> ID: %d, Configs: %d\n", id, num_configs);
                    }
                    
                    nuevosDispositivos[i].id = id;
                    strcpy(nuevosDispositivos[i].nombre, nombre);
                    nuevosDispositivos[i].num_configs = num_configs;
                    nuevosDispositivos[i].configs = (configuracion*)malloc(num_configs * sizeof(configuracion));
                    //insertDispositivoDB(db, nuevosDispositivos[i], cliente_encontrado->id);

                    for (int j = 0; j < num_configs; j++) {
                        recibirMensajeServidor(client_socket, buffer, sizeof(buffer));
                        int version;
                        char ruta[256]; // [Longitud: 256 caracteres para la ruta original de config]
                        char fecha[50]; // [Longitud: 50 caracteres para la fecha de config]
                        char rutaNU[256];
                        sscanf(buffer, "CONFIG %d %s %s", &version, rutaNU, fecha);
                        sprintf(ruta, "src/confs_cliente/%s",rutaNU);
                        printf("%s\n",buffer);
                        printf("%s\n",ruta);
                        nuevosDispositivos[i].configs[j].version = version;
                        strcpy(nuevosDispositivos[i].configs[j].ruta, ruta);
                        strcpy(nuevosDispositivos[i].configs[j].fecha, fecha);

                    
                        //insertConfiguracionDB(db,nuevosDispositivos[i].configs[j], nuevosDispositivos[i].id );

                        recibirMensajeServidor(client_socket, buffer, sizeof(buffer));
                        long fileSize;
                        sscanf(buffer, "FILE_SIZE %ld", &fileSize);

                        if (fileSize > 0) {
                            char rutaDestino[512]; 
                            sprintf(rutaDestino, "%s", ruta);
                            
                            FILE* file = fopen(rutaDestino, "wb");
                            long bytesRestantes = fileSize;
                            char fileBuffer[1024]; 

                            if (file) {
                                
                                while (bytesRestantes > 0) {
                                    int bytesALeer = (bytesRestantes < sizeof(fileBuffer)) ? bytesRestantes : sizeof(fileBuffer);
                                    int bytesLeidos = recv(client_socket, fileBuffer, bytesALeer, 0);
                                    if (bytesLeidos > 0) {
                                        fwrite(fileBuffer, 1, bytesLeidos, file);
                                        bytesRestantes -= bytesLeidos;
                                    } else break;
                                }
                                fclose(file);
                            } else {
                                
                                printf("Error: No se pudo abrir %s. Descartando %ld bytes de la red...\n", rutaDestino, fileSize);
                                
                                
                                while (bytesRestantes > 0) {
                                    int bytesALeer = (bytesRestantes < sizeof(fileBuffer)) ? bytesRestantes : sizeof(fileBuffer);
                                    int bytesLeidos = recv(client_socket, fileBuffer, bytesALeer, 0);
                                    if (bytesLeidos > 0) {
                                        bytesRestantes -= bytesLeidos;
                                    } else break;
                                }
                            }
                        }
                    }
                    insertDispositivoDB(db, nuevosDispositivos[i], cliente_encontrado->id);
                }
                recibirMensajeServidor(client_socket, buffer, sizeof(buffer)); // Leer "END"

                // TODO:BASE DE DATOS

                detectarDispositivosEliminados(cliente_encontrado->listaDispositivos, cliente_encontrado->numDispositivos, 
                                   nuevosDispositivos,numNuevos,
                                   cliente_encontrado->id, db);

                
                cliente_encontrado->listaDispositivos = nuevosDispositivos;
                cliente_encontrado->numDispositivos = numNuevos;


                
                printf("Sincronizacion de datos y archivos completada.\n");
                
                // printf("DDDDDDDDD:   %d \n",cliente_encontrado->numDispositivos );
                // printf("DDDDDDDDD:   %d \n",cliente_encontrado->listaDispositivos[1].num_configs);
                // printf("RRRR  %s\n", cliente_encontrado->listaDispositivos[1].configs[1].ruta);

                for(int i = 0; i < cliente_encontrado->numDispositivos; i++){
                    for(int j = 0; j < cliente_encontrado->listaDispositivos[i].num_configs; j ++){
                        printf("RRRR  %s\n", cliente_encontrado->listaDispositivos[i].configs[j].ruta);
                        printf("AAAAA\n");
                    }
                }
                //printf("RRRR  %s\n", cliente_encontrado->listaDispositivos[1].configs[1].ruta);
            }
        } // Fin de while(clienteActivo)

        closesocket(client_socket); // Se cierra solo cuando el cliente finaliza la sesión
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}