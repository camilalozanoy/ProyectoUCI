#include "anomalias.h"

int buscarUmbral(const Umbral* lista, int total,
                 const char* nombreBuscado, double& minimo, double& maximo) {
    int i = 0;
    while (i < total) {
        int iguales = 1;
        int k = 0;
        while (k < 6) {
            char a = lista[i].tipoSensor[k];
            char b = nombreBuscado[k];
            if (a != b) { iguales = 0; break; }
            if (a == '\0' && b == '\0') break; 
            k = k + 1;
        }
        if (iguales) {
            minimo = lista[i].umbralMin;
            maximo = lista[i].umbralMax;
            return 1;
        }
        i = i + 1;
    }
    return 0;
}

bool detectarAnomalia(const Lectura& lectura, const Umbral* configuraciones, int numConfigs)
{
    if (lectura.tipoSensor == 'E') return false;

    if (lectura.tipoSensor == 'T') {
        double minT = 0.0, maxT = 0.0;
        if (buscarUmbral(configuraciones, numConfigs, "T", minT, maxT)) {
            double v = lectura.valor[0];
            if (v < minT) return true;
            if (v > maxT) return true;
        }
        return false;
    }

    if (lectura.tipoSensor == 'O') {
        double minO = 0.0, maxO = 0.0;
        if (buscarUmbral(configuraciones, numConfigs, "O", minO, maxO)) {
            double v = lectura.valor[0];
            if (v < minO) return true;
            if (v > maxO) return true;
        }
        return false;
    }

    if (lectura.tipoSensor == 'P') {
        double minSis = 0.0, maxSis = 0.0;
        double minDia = 0.0, maxDia = 0.0;
        int tieneSis = buscarUmbral(configuraciones, numConfigs, "P_SIS", minSis, maxSis);
        int tieneDia = buscarUmbral(configuraciones, numConfigs, "P_DIA", minDia, maxDia);
        if (tieneSis && tieneDia) {
            double limInf = 0.5 * (minSis + minDia);
            double limSup = 0.5 * (maxSis + maxDia);
            double avg = 0.5 * (lectura.valor[0] + lectura.valor[1]);
            if (avg < limInf) return true;
            if (avg > limSup) return true;
        }
        return false;
    }

    return false;
}