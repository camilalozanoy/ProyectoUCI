#ifndef CARGA_H
#define CARGA_H
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "structs.h"


Umbral* separarConfiguracion(const char* linea) {
    char buffer[100];
    strcpy(buffer, linea);
    Umbral* umbral= new Umbral;
    strcpy(umbral->tipoSensor, strtok(buffer, ","));
    char* token = strtok(nullptr, ",");
    umbral->umbralMin = atof(token);

    token = strtok(nullptr, ",");
    umbral->umbralMax = atof(token);
    return umbral;
}
Configuracion* cargarConfiguracion(const char* archivo) {
    
    Configuracion* configuracion= new Configuracion;
    std::ifstream file(archivo);
    char linea[100];
    int numConfigs = 0;
    if (file.fail()){
        return nullptr;
    }
    while (file.getline(linea, 100)) {
        Umbral* nuevoUmbral= separarConfiguracion(linea);
            strcpy (configuracion->umbrales[numConfigs].tipoSensor, nuevoUmbral->tipoSensor);
            configuracion->umbrales[numConfigs].umbralMax= nuevoUmbral->umbralMax;
            configuracion->umbrales[numConfigs].umbralMin= nuevoUmbral->umbralMin;
            configuracion->tam++;
            numConfigs++;
            delete nuevoUmbral;
    }
    file.close();
    return configuracion;
}


#endif
