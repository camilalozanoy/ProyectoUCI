#include "lecturaBSF.h"
#include <fstream>
#include <iostream>
#include <cstring>
using namespace std;

int byteAIntPositivo(char b) {
    int x = (int)b;
    if (x < 0) {
        x = x + 256;
    }
    return x;
}

void leerArchivoBSF(const char* archivo, SalaUCI*& salas, int& numSalas) {
    salas = NULL;
    numSalas = 0;

    std::ifstream file(archivo, std::ios::binary);
    if (!file) {
        std::cout << "No se pudo abrir el archivo " << archivo << "\n";
        return;
    }

    char idSalaByte = 0;
    char nMaquinasByte = 0;

    file.read(&idSalaByte, 1);
    file.read(&nMaquinasByte, 1);
    if (!file) {
        std::cout << "Archivo BSF inválido (cabecera de sala)\n";
        return;
    }

    int nMaquinas = byteAIntPositivo(nMaquinasByte);

    numSalas = 1;
    salas = new SalaUCI[numSalas];
    salas[0].idSala = idSalaByte;
    salas[0].numMaquinas = nMaquinas;
    salas[0].maquinas = new Maquina[nMaquinas];

    int i = 0;
    while (i < nMaquinas) {
        Maquina& M = salas[0].maquinas[i];

        char idMByte = 0;
        int numMed = 0;

        file.read(&idMByte, 1);
        file.read((char*)&numMed, sizeof(numMed));
        if (!file) {
        cout << "Archivo BSF inválido (cabecera de máquina)\n";
            return;
        }

        M.idMaquina = idMByte;
        M.numMediciones = numMed;
        M.mediciones = new Medicion[numMed];

        int j = 0;
        while (j < numMed) {
            Medicion& md = M.mediciones[j];

            char idPByte = 0;
            char fecha[24];
            int nLect = 0;

            file.read(&idPByte, 1);
            file.read(fecha, 24);
            file.read((char*)&nLect, sizeof(nLect)); 
            if (!file) {
            cout << "Archivo BSF inválido (cabecera de medición)\n";
                return;
            }

            fecha[23] = '\0';

            int x = 0;
            while (x < 11) { md.idPaciente[x] = '\0'; x = x + 1; }
            md.idPaciente[0] = idPByte;
            md.idPaciente[1] = '\0';

            std::memcpy(md.fechaHora, fecha, 24);
            md.numLecturas = nLect;
            md.lecturas = new Lectura[nLect];

            int k = 0;
            while (k < nLect) {
                Lectura& L = md.lecturas[k];

                char tipoByte = 0;
                file.read(&tipoByte, 1);
                if (!file) {
                    cout << "Archivo BSF invalido (tipo de lectura)\n";
                    return;
                }
                L.tipoSensor = tipoByte;

                if (L.tipoSensor == 'T' || L.tipoSensor == 'E' || L.tipoSensor == 'O') {
                    double val = 0.0;
                    file.read((char*)&val, sizeof(val));  
                    if (!file) {
                        cout << "Archivo BSF invalido (valor double)\n";
                        return;
                    }
                    L.valor[0] = val;
                    L.valor[1] = 0.0;
                } else if (L.tipoSensor == 'P') {
                    float ps = 0.0f;
                    float pd = 0.0f;
                    file.read((char*)&ps, sizeof(ps));     
                    file.read((char*)&pd, sizeof(pd));     
                    if (!file) {
                        cout << "Archivo BSF inválido (par de floats de presión)\n";
                        return;
                    }
                    L.valor[0] = (double)ps;
                    L.valor[1] = (double)pd;
                } else {
                    int tipoNum = byteAIntPositivo(tipoByte);
                    cout << "Tipo de sensor desconocido: " << tipoNum << "\n";
                    return;
                }

                k = k + 1;
            }

            j = j + 1;
        }

        i = i + 1;
    }
}