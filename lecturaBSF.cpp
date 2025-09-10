#include "lecturaBSF.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdint>

using namespace std;

int byteAIntPositivo(char b) {
    int x = (int)b;
    if (x < 0) x += 256;
    return x;
}

bool read_u8(ifstream& f, unsigned char& out) {
    char b = 0;
    f.read(&b, 1);
    if (!f) return false;
    out = (unsigned char)b;
    return true;
}
bool read_u16le(ifstream& f, uint16_t& out) {
    unsigned char b[2] = {0,0};
    f.read(reinterpret_cast<char*>(b), 2);
    if (!f) return false;
    out = (uint16_t)( (uint16_t)b[0] | ((uint16_t)b[1] << 8) );
    return true;
}
bool read_u32le(ifstream& f, uint32_t& out) {
    unsigned char b[4] = {0,0,0,0};
    f.read(reinterpret_cast<char*>(b), 4);
    if (!f) return false;
    out = (uint32_t)( (uint32_t)b[0]
                    | ((uint32_t)b[1] << 8)
                    | ((uint32_t)b[2] << 16)
                    | ((uint32_t)b[3] << 24) );
    return true;
}


int read_count_flexible(ifstream& f, int max_reasonable) {
    std::streampos pos = f.tellg();

    unsigned char c8 = 0;
    f.clear(); f.seekg(pos);
    if (read_u8(f, c8)) {
        int v = (int)c8;
        if (v >= 0 && v <= max_reasonable) return v;
    }

    uint16_t c16 = 0;
    f.clear(); f.seekg(pos);
    if (read_u16le(f, c16)) {
        int v = (int)c16;
        if (v >= 0 && v <= max_reasonable) return v;
    }

    uint32_t c32 = 0;
    f.clear(); f.seekg(pos);
    if (read_u32le(f, c32)) {
        int v = (int)c32;
        if (v >= 0 && v <= max_reasonable) return v;
    }

    f.clear(); f.seekg(pos);
    return -1;
}

void leerArchivoBSF(const char* archivo, SalaUCI*& salas, int& numSalas) {
    salas = NULL;
    numSalas = 0;

    ifstream file(archivo, ios::binary);
    if (!file) {
        cout << "No se pudo abrir el archivo " << archivo << "\n";
        return;
    }

    char idSalaByte = 0;
    char nMaquinasByte = 0;

    file.read(&idSalaByte, 1);
    file.read(&nMaquinasByte, 1);
    if (!file) {
        cout << "Archivo BSF invalido (cabecera de sala)\n";
        return;
    }

    int nMaquinas = byteAIntPositivo(nMaquinasByte);
    if (nMaquinas < 0 || nMaquinas > 10000) {
        cout << "Archivo BSF invalido (nMaquinas fuera de rango)\n";
        return;
    }

    numSalas = 1;
    salas = new SalaUCI[numSalas]();
    salas[0].idSala = idSalaByte;
    salas[0].numMaquinas = nMaquinas;
    salas[0].maquinas = (nMaquinas > 0) ? new Maquina[nMaquinas]() : NULL;

    int i = 0;
    while (i < nMaquinas) {
        Maquina& M = salas[0].maquinas[i];

        char idMByte = 0;
        file.read(&idMByte, 1);
        if (!file) {
            cout << "Archivo BSF inválido (id de máquina)\n";
            return;
        }

        int numMed = read_count_flexible(file, 1000000);
        if (numMed < 0) {
            cout << "Archivo BSF invalido (numMed no legible)\n";
            return;
        }

        M.idMaquina = idMByte;
        M.numMediciones = numMed;
        M.mediciones = (numMed > 0) ? new Medicion[numMed]() : NULL;

        int j = 0;
        while (j < numMed) {
            Medicion& md = M.mediciones[j];

            char idPByte = 0;
            file.read(&idPByte, 1);
            if (!file) { cout << "Archivo BSF invalido (id paciente)\n"; return; }

            char fecha[24];
            file.read(fecha, 23);
            if (!file) { cout << "Archivo BSF invalido (fecha)\n"; return; }
            fecha[23] = '\0';

            int nLect = read_count_flexible(file, 1000000);
            if (nLect < 0) {
                cout << "Archivo BSF invalido (nLect no legible)\n";
                return;
            }

            int x = 0;
            while (x < 11) { md.idPaciente[x] = '\0'; x = x + 1; }
            md.idPaciente[0] = idPByte;
            md.idPaciente[1] = '\0';
            std::memcpy(md.fechaHora, fecha, 24);

            md.numLecturas = nLect;
            md.lecturas = (nLect > 0) ? new Lectura[nLect]() : NULL;

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

                if (L.tipoSensor == 'T' || L.tipoSensor == 'O' || L.tipoSensor == 'E') {
                    float valf = 0.0f;
                    file.read(reinterpret_cast<char*>(&valf), sizeof(valf));
                    if (!file) {
                        cout << "Archivo BSF invalido (valor float)\n";
                        return;
                    }
                    L.valor[0] = (double)valf;
                    L.valor[1] = 0.0;
                } else if (L.tipoSensor == 'P') {
                    float ps = 0.0f;
                    float pd = 0.0f;
                    file.read(reinterpret_cast<char*>(&ps), sizeof(ps));
                    file.read(reinterpret_cast<char*>(&pd), sizeof(pd));
                    if (!file) {
                        cout << "Archivo BSF inválido (presión)\n";
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