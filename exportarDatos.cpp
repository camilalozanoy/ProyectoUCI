#include <fstream>
#include <cstring>
#include "exportarDatos.h"
#include "anomalias.h"
#include "estadisticas.h"

using namespace std;

void exportarDatosProcesados(const SalaUCI* salas, int numSalas, const Umbral* configuraciones, int numConfigs) {
    ofstream out("datos_procesados.txt");
    if (!out) return;

    for (int s = 0; s < numSalas; ++s) {
        out << "Sala: " << salas[s].idSala << "\n";
        for (int m = 0; m < salas[s].numMaquinas; ++m) {
            const Maquina& maq = salas[s].maquinas[m];
            out << "  Maquina: " << maq.idMaquina << "\n";

            for (int md = 0; md < maq.numMediciones; ++md) {
                const Medicion& med = maq.mediciones[md];
                out << "    Paciente: " << med.idPaciente
                    << " | Fecha: " << med.fechaHora << "\n";

                for (int l = 0; l < med.numLecturas; ++l) {
                    const Lectura& lec = med.lecturas[l];

                    bool anom = detectarAnomalia(lec, configuraciones, numConfigs);

                    out << "      Sensor: " << lec.tipoSensor
                        << " | Valor: " << lec.valor
                        << " | Estado: " << (anom ? "ANOMALO" : "NORMAL") << "\n";
                }

                double minV, maxV, promV;
                const char* id = med.idPaciente;

                calcularEstadisticas(salas, numSalas, id, minV, maxV, promV, 'T');
                out << "      [T] Min: " << minV << " Max: " << maxV << " Prom: " << promV << "\n";

                calcularEstadisticas(salas, numSalas, id, minV, maxV, promV, 'P');
                out << "      [P] Min: " << minV << " Max: " << maxV << " Prom: " << promV << "\n";

                calcularEstadisticas(salas, numSalas, id, minV, maxV, promV, 'O');
                out << "      [O] Min: " << minV << " Max: " << maxV << " Prom: " << promV << "\n";
            }
        }
    }
}
