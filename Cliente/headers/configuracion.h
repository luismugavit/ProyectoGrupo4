#ifndef CONFIGURACION_H
#define CONFIGURACION_H

class Configuracion {

    

public:
    int version;
    char ruta[256];
    char fecha[50];
    
    Configuracion();
    Configuracion(int version, const char* ruta, const char* fecha);
    ~Configuracion();


    int getVersion() const;
    const char* getRuta() const;
    const char* getFecha() const;
    Configuracion& operator=(const Configuracion& other);
    void setVersion(int version);
    void setRuta(const char* ruta);
    void setFecha(const char* fecha);

    void mostrar() const;
};

#endif
