#include <iostream>
#include <fstream>
#include <cstring>

#include "structs.h"
#include "carga.h"
#include "reportes.h"
#include "estadisticas.h"
#include "anomalias.h"
#include "exportarDatos.h"
#include "lecturaBSF.h"

#include "lecturaBSF.cpp"
#include "reportes.cpp"
#include "estadisticas.cpp"
#include "exportarDatos.cpp"
#include "anomalias.cpp"

using namespace std;

void liberarSalas(SalaUCI*& salas, int& numSalas) {
    if (!salas || numSalas <= 0) {
        salas = NULL;
        numSalas = 0;
        return;
    }

    int s = 0;
    while (s < numSalas) {
        SalaUCI& sala = salas[s];

        int m = 0;
        while (m < sala.numMaquinas) {
            Maquina& maq = sala.maquinas[m];

            int md = 0;
            while (md < maq.numMediciones) {
                Medicion& med = maq.mediciones[md];
                if (med.lecturas) {
                    delete[] med.lecturas;
                    med.lecturas = NULL;
                }
                md = md + 1;
            }

            if (maq.mediciones) {
                delete[] maq.mediciones;
                maq.mediciones = NULL;
            }

            m = m + 1;
        }

        if (sala.maquinas) {
            delete[] sala.maquinas;
            sala.maquinas = NULL;
        }

        s = s + 1;
    }

    delete[] salas;
    salas = NULL;
    numSalas = 0;
}

int main() {
    Configuracion* configuracion = cargarConfiguracion("configuracion.txt");
    SalaUCI* salas = NULL;
    int numSalas = 0;

    int opcion;
    do {
        cout << "\nMenu \n";
        cout << "1. Mostrar cantidades cargadas\n";
        cout << "2. Generar reporte de mediciones por paciente\n";
        cout << "3. Calcular estadisticas (por paciente y sensor)\n";
        cout << "4. Exportar datos procesados\n";
        cout << "5. Leer archivo .bsf (reemplaza datos en memoria)\n";
        cout << "6. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            if (configuracion) {
                cout << "Configuraciones: " << configuracion->tam << "\n";
            } else {
                cout << "Configuraciones: 0 (no cargadas)\n";
            }
            if (salas && numSalas > 0) {
                int totalMed = 0;
                int s = 0;
                while (s < numSalas) {
                    int m = 0;
                    while (m < salas[s].numMaquinas) {
                        totalMed = totalMed + salas[s].maquinas[m].numMediciones;
                        m = m + 1;
                    }
                    s = s + 1;
                }
                cout << "Salas: " << numSalas << " | Total mediciones: " << totalMed << "\n";
            } else {
                cout << "No hay datos BSF cargados.\n";
            }
        }
        else if (opcion == 2) {
            char idPaciente[11];
            cout << "Ingrese el ID del paciente: ";
            cin >> idPaciente;

            if (!salas || numSalas == 0) {
                cout << "Primero cargue un .bsf (opcion 5).\n";
            } else if (!configuracion) {
                cout << "No hay configuracion cargada.\n";
            } else {
                generarReportePaciente(
                    idPaciente,
                    salas,
                    numSalas,
                    configuracion->umbrales,
                    configuracion->tam
                );
                cout << "Reporte generado en 'mediciones_paciente_" << idPaciente << ".txt'\n";
            }
        }
        else if (opcion == 3) {
            char idPaciente[11];
            char tipoSensor;
            cout << "Ingrese el ID del paciente: ";
            cin >> idPaciente;
            cout << "Tipo de sensor (T/P/O/E): ";
            cin >> tipoSensor;

            if (!salas || numSalas == 0) {
                cout << "Primero cargue un .bsf (opcion 5).\n";
            } else {
                double minV = 0.0, maxV = 0.0, promV = 0.0;
                calcularEstadisticas(salas, numSalas, idPaciente, minV, maxV, promV, tipoSensor);

                cout << "Estadisticas para paciente " << idPaciente << " y sensor " << tipoSensor << ":\n";
                cout << "Min: " << minV << " | Max: " << maxV << " | Prom: " << promV << "\n";
            }
        }
        else if (opcion == 4) {
            if (!salas || numSalas == 0) {
                cout << "Primero cargue un .bsf (opcion 5).\n";
            } else if (!configuracion) {
                cout << "No hay configuracion cargada.\n";
            } else {
                exportarDatosProcesados(
                    salas,
                    numSalas,
                    configuracion->umbrales,
                    configuracion->tam
                );
                cout << "Exportado a 'datos_procesados.txt'\n";
            }
        }
        else if (opcion == 5) {
            char ruta[256];
            cout << "Ingrese nombre del archivo .bsf (ej. patient_readings_simulation_small.bsf): ";
            cin >> ruta;

            liberarSalas(salas, numSalas);

            leerArchivoBSF(ruta, salas, numSalas);

            if (salas && numSalas > 0) {
                cout << "BSF cargado. Salas: " << numSalas << "\n";
                int s = 0;
                while (s < numSalas) {
                    cout << "  Sala " << salas[s].idSala
                         << " con " << salas[s].numMaquinas << " maquinas.\n";
                    int m = 0;
                    while (m < salas[s].numMaquinas) {
                        cout << "    Maq " << salas[s].maquinas[m].idMaquina
                             << " con " << salas[s].maquinas[m].numMediciones << " mediciones.\n";
                        m = m + 1;
                    }
                    s = s + 1;
                }
            } else {
                cout << "No se pudo leer el archivo BSF.\n";
            }
        }
        else if (opcion == 6) {
            cout << "Chao profe\n";
        }
        else {
            cout << "Opcion invalida.\n";
        }

    } while (opcion != 6);

    liberarSalas(salas, numSalas);
    if (configuracion) {
        delete configuracion;
        configuracion = NULL;
    }
    return 0;
}