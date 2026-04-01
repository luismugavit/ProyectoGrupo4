#ifndef INTERFAZ_H
#define INTERFAZ_H

void registrarOperacion(const char *usuario, const char *operacion);
void crearCliente();
void anadirConfiguracion();
void infoCliente(cliente cl);
void listarDispositivos();
void ListarClientes();
void eliminarClienteUI();
void MostrarMenuGestionClientes();

int MostrarMenuGestionDispositivos();

int MostrarMenuPrincipal();
void login();

#endif
