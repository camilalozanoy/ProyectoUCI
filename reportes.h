#ifndef REPORTES_H
#define REPORTES_H

#include "structs.h"

void ordenarMedicionesPorFecha(Medicion* mediciones, int numMediciones);

void generarReportePaciente(const char* idPaciente, SalaUCI* salas, int numSalas, const Umbral* configuraciones, int numConfigs);

#endif
