#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <winsock2.h>

int recibirMensajeB(SOCKET sock, char* buffer, int max_len);
const char* obtenerNombreArchivoB(const char* ruta);
int conectarServidorB();


#endif