#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include "structs.h"

void calcularEstadisticas(const SalaUCI* salas, int numSalas, const char* idPaciente, double& minV, double& maxV, double& promV, char tipoSensor);

#endif
