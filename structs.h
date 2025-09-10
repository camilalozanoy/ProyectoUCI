#ifndef STRUCTS_H
#define STRUCTS_H


struct Umbral {
    char tipoSensor [6];
    double umbralMin;
    double umbralMax;
};

struct Configuracion {
    Umbral umbrales [5];
    int tam=0;
};

struct Paciente {
    char id[11];
    char tipoDocumento[5];
    char documento[20];
    char nombres[50];
    char apellidos[50];
    char fechaNacimiento[11];
    char telefono[15];
    char email[50];
    char tipoSangre[5];
    char entidadSalud[50];
    char medicinaPrepagada[50];
};

struct ListaPacientes{
    Paciente* pacientes=nullptr;
    int tam=0;
};

struct Lectura {
    char tipoSensor;
    double valor[2];
};

struct Medicion {
    char idPaciente[11];
    char fechaHora[24];
    int numLecturas;
    Lectura* lecturas;
};

struct Maquina {
    char idMaquina;
    int numMediciones;
    Medicion* mediciones;
};

struct SalaUCI {
    char idSala;
    int numMaquinas;
    Maquina* maquinas;
};

#endif
