#include <iostream>
#include <fstream>
#include <cstring>

#include "structs.h"
#include "carga.h"
#include "reportes.h"
#include "estadisticas.h"
#include "anomalias.h"
#include "exportarDatos.h"
#include "utiles.h"
#include "lecturaBSF.h"

#include "lecturaBSF.cpp"
#include "reportes.cpp"
#include "estadisticas.cpp"
#include "exportarDatos.cpp"
#include "anomalias.cpp"

using namespace std;

int main() {
    
    Configuracion* configuracion= cargarConfiguracion("configuracion.txt");
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
            cout << "Configuraciones: "<< configuracion->tam <<"\n";
            cout << "Pacientes: " <<"\n";
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

            double minV = 0, maxV = 0, promV = 0;
            calcularEstadisticas(salas, numSalas, idPaciente, minV, maxV, promV, tipoSensor);

            cout << "Estadisticas para paciente " << idPaciente << " y sensor " << tipoSensor << ":\n";
            cout << "Min: " << minV << " | Max: " << maxV << " | Prom: " << promV << "\n";
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

            leerArchivoBSF(ruta, salas, numSalas);

            if (salas && numSalas > 0) {
                cout << "BSF cargado. Salas: " << numSalas << "\n";
                for (int s = 0; s < numSalas; ++s) {
                    cout << "  Sala " << salas[s].idSala
                         << " con " << salas[s].numMaquinas << " maquinas.\n";
                    for (int m = 0; m < salas[s].numMaquinas; ++m) {
                        cout << "    Maq " << salas[s].maquinas[m].idMaquina
                             << " con " << salas[s].maquinas[m].numMediciones << " mediciones.\n";
                    }
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

    delete configuracion;

    return 0; }