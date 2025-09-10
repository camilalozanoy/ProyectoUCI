#ifndef ANOMALIAS_H
#define ANOMALIAS_H

#include "structs.h"

bool detectarAnomalia(const Lectura& lectura,
                      const Umbral* configuraciones, int numConfigs);

#endif
