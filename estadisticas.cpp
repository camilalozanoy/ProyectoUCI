#include <cstring>
#include "estadisticas.h"

void calcularEstadisticas(const SalaUCI* salas, int numSalas, const char* idPaciente, double& minV, double& maxV, double& promV, char tipoSensor)
{
    bool init = false;
    double suma = 0.0;
    int cnt = 0;

    minV = 0.0; maxV = 0.0; promV = 0.0;
    if (!salas || numSalas <= 0 || !idPaciente) return;

    const char target = idPaciente[0];

    int s = 0;
    while (s < numSalas) {
        const SalaUCI& sala = salas[s];
        if (!sala.maquinas || sala.numMaquinas <= 0) { s = s + 1; continue; }

        int m = 0;
        while (m < sala.numMaquinas) {
            const Maquina& maq = sala.maquinas[m];
            if (!maq.mediciones || maq.numMediciones <= 0) { m = m + 1; continue; }

            int md = 0;
            while (md < maq.numMediciones) {
                const Medicion& med = maq.mediciones[md];

                if (med.idPaciente[0] != target) { md = md + 1; continue; }

                if (med.numLecturas <= 0 || !med.lecturas) { md = md + 1; continue; }

                int l = 0;
                while (l < med.numLecturas) {
                    const Lectura& lec = med.lecturas[l];
                    if (lec.tipoSensor != tipoSensor) { l = l + 1; continue; }

                    double x = 0.0;
                    if (tipoSensor == 'P') {
                        x = 0.5 * (lec.valor[0] + lec.valor[1]);
                    } else {
                        x = lec.valor[0];
                    }

                    if (!init) { minV = x; maxV = x; init = true; }
                    if (x < minV) minV = x;
                    if (x > maxV) maxV = x;
                    suma += x;
                    cnt = cnt + 1;

                    l = l + 1;
                }

                md = md + 1;
            }

            m = m + 1;
        }

        s = s + 1;
    }

    if (cnt > 0) promV = suma / (double)cnt;
}