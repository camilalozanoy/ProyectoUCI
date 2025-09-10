#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "carga.h"
using namespace std;

void separarConfiguracion(const char* linea, Umbral& config) {
    char buffer[100];
    strcpy(buffer, linea);

    config.tipoSensor = buffer[0];
    char* token = strtok(buffer + 2, ",");
    config.umbralMin = atof(token);

    token = strtok(nullptr, ",");
    config.umbralMax = atof(token);
}

Umbral& separarConfiguracion(const char* linea) {
    char buffer[100];
    strcpy(buffer, linea);
    Umbral umbral;
    umbral.tipoSensor = strtok(buffer, ",");
    char* token = strtok(nullptr, ",");
    umbral.umbralMin = atof(token);

    token = strtok(nullptr, ",");
    umbral.umbralMax = atof(token);
    return umbral;
}

void separarPaciente(const char* linea, Paciente& paciente) {
    char buffer[500];
    strcpy(buffer, linea);

    char* token = strtok(buffer, ";");
    strcpy(paciente.id, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.tipoDocumento, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.documento, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.nombres, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.apellidos, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.fechaNacimiento, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.telefono, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.email, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.tipoSangre, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.entidadSalud, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.medicinaPrepagada, token);
}

Paciente& separarPaciente(const char* linea) {
    char buffer[500];
    strcpy(buffer, linea);
    Paciente paciente;

    char* token = strtok(buffer, ";");
    strcpy(paciente.id, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.tipoDocumento, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.documento, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.nombres, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.apellidos, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.fechaNacimiento, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.telefono, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.email, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.tipoSangre, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.entidadSalud, token);

    token = strtok(nullptr, ";");
    strcpy(paciente.medicinaPrepagada, token);
    return paciente;
}

Configuracion& cargarConfiguracion(const char* archivo) {
    
    Configuracion configuracion;
    ifstream file(archivo);
    char linea[100];
    numConfigs = 0;
    if (file.fail()){
        return configuracion;
    }
    while (file.getline(linea, 100)) {
            configuracion.umbrales[numConfigs] = separarConfiguracion(linea);
            configuracion.tam++;
            numConfigs++;
    }
    file.close();
    return configuracion;
}

ListaPacientes* cargarPacientes(const char* archivo) {
    ifstream file(archivo);
    char linea[500];
    ListaPacientes listaPacientes= new ListaPacientes;
    if (file.fail()){
        return nullptr;
    }
    while (file.getline(linea, 500)) {
        
            if(listaPacientes.pacientes==nullptr){
                listaPacientes.pacientes= new Paciente[1];
            }else{
                Paciente* aux= listaPacientes.pacientes;
                listaPacientes.pacientes= new Paciente [listaPacientes.tam+1];
                for (int i=0; i<listaPacientes.tam; i++){
                    listaPacientes.pacientes[i]= aux[i];
                }
                delete []aux;
            }
            Paciente& paciente=separarPaciente(linea);
            listaPacientes.pacientes[listaPacientes.tam]= paciente;
            listaPacientes.tam++
        }
    file.close();
    return listaPacientes;
    
}
