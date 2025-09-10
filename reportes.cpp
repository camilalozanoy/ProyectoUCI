#include <fstream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include "reportes.h"

int parseFecha(const char* s, int& D,int& M,int& Y,int& h,int& m,int& sec,int& ms){
    int ok = std::sscanf(s, "%2d/%2d/%4d %2d:%2d:%2d.%3d", &D,&M,&Y,&h,&m,&sec,&ms);
    if (ok == 7) return 1;
    return 0;
}

int cmpFecha(const char* a, const char* b){
    int D1,M1,Y1,h1,m1,s1,ms1;
    int D2,M2,Y2,h2,m2,s2,ms2;
    if (!parseFecha(a,D1,M1,Y1,h1,m1,s1,ms1)) return 0;
    if (!parseFecha(b,D2,M2,Y2,h2,m2,s2,ms2)) return 0;
    if (Y1!=Y2) { if (Y1<Y2) return -1; else return 1; }
    if (M1!=M2) { if (M1<M2) return -1; else return 1; }
    if (D1!=D2) { if (D1<D2) return -1; else return 1; }
    if (h1!=h2) { if (h1<h2) return -1; else return 1; }
    if (m1!=m2) { if (m1<m2) return -1; else return 1; }
    if (s1!=s2) { if (s1<s2) return -1; else return 1; }
    if (ms1!=ms2){ if (ms1<ms2) return -1; else return 1; }
    return 0;
}

void ordenarMedicionesPorFecha(Medicion* mediciones, int numMediciones) {
    int i = 0;
    while (i < numMediciones - 1) {
        int j = 0;
        while (j < numMediciones - i - 1) {
            if (cmpFecha(mediciones[j].fechaHora, mediciones[j + 1].fechaHora) > 0) {
                Medicion t = mediciones[j];
                mediciones[j] = mediciones[j + 1];
                mediciones[j + 1] = t;
            }
            j = j + 1;
        }
        i = i + 1;
    }
}

int getUmbral(const Umbral* cfg, int n, const char* tipo, double& umin, double& umax){
    int i = 0;
    while (i < n) {
        int iguales = 1;
        int k = 0;
        while (k < 6) {
            char a = cfg[i].tipoSensor[k];
            char b = tipo[k];
            if (a != b) { iguales = 0; break; }
            if (a == '\0' && b == '\0') break;
            k = k + 1;
        }
        if (iguales) {
            umin = cfg[i].umbralMin;
            umax = cfg[i].umbralMax;
            return 1;
        }
        i = i + 1;
    }
    return 0;
}

int esAnomalaLectura(const Lectura& L, const Umbral* cfg, int n){
    if (L.tipoSensor == 'E') return 0;
    if (L.tipoSensor == 'T') {
        double a=0.0,b=0.0;
        if(!getUmbral(cfg,n,"T",a,b)) return 0;
        double v = L.valor[0];
        if (v<a) return 1;
        if (v>b) return 1;
        return 0;
    }
    if (L.tipoSensor == 'O') {
        double a=0.0,b=0.0;
        if(!getUmbral(cfg,n,"O",a,b)) return 0;
        double v = L.valor[0];
        if (v<a) return 1;
        if (v>b) return 1;
        return 0;
    }
    if (L.tipoSensor == 'P') {
        double smin=0.0,smax=0.0,dmin=0.0,dmax=0.0;
        int hs = getUmbral(cfg,n,"P_SIS",smin,smax);
        int hd = getUmbral(cfg,n,"P_DIA",dmin,dmax);
        if (hs && hd) {
            double avgMin = 0.5*(smin + dmin);
            double avgMax = 0.5*(smax + dmax);
            double avg = 0.5*(L.valor[0] + L.valor[1]);
            if (avg<avgMin) return 1;
            if (avg>avgMax) return 1;
            return 0;
        }
        return 0;
    }
    return 0;
}

struct PtrLect { const Medicion* md; const Lectura* L; };

void sort_ptrs(PtrLect* a, int n){
    int i = 1;
    while (i < n) {
        PtrLect key = a[i]; int j = i - 1;
        while (j >= 0 && cmpFecha(a[j].md->fechaHora, key.md->fechaHora) > 0) {
            a[j+1] = a[j]; j = j - 1;
        }
        a[j+1] = key; i = i + 1;
    }
}

void generarReportePaciente(const char* idPaciente, SalaUCI* salas, int numSalas, const Umbral* configuraciones, int numConfigs) {
    char nombre[128];
    std::snprintf(nombre, sizeof(nombre), "mediciones_paciente_%s.txt", idPaciente);
    std::ofstream out(nombre);
    if (!out.is_open()) return;

    char target = idPaciente[0];
    int cT=0, cO=0, cP=0, cE=0;
    int s = 0;

    while (s < numSalas) {
        SalaUCI& sala = salas[s];
        int i = 0;
        while (i < sala.numMaquinas) {
            Maquina& M = sala.maquinas[i];
            int j = 0;
            while (j < M.numMediciones) {
                Medicion& md = M.mediciones[j];
                if (md.idPaciente[0] == target) {
                    int k = 0;
                    while (k < md.numLecturas) {
                        Lectura& L = md.lecturas[k];
                        if      (L.tipoSensor=='T') cT = cT + 1;
                        else if (L.tipoSensor=='O') cO = cO + 1;
                        else if (L.tipoSensor=='P') cP = cP + 1;
                        else if (L.tipoSensor=='E') cE = cE + 1;
                        k = k + 1;
                    }
                }
                j = j + 1;
            }
            i = i + 1;
        }
        s = s + 1;
    }

    if (cT==0 && cO==0 && cP==0 && cE==0) {
        out << "Paciente no encontrado.\n";
        out.close();
        return;
    }

    PtrLect* aT = NULL; int nT=0;
    PtrLect* aO = NULL; int nO=0;
    PtrLect* aP = NULL; int nP=0;
    PtrLect* aE = NULL; int nE=0;

    if (cT > 0) aT = new PtrLect[cT];
    if (cO > 0) aO = new PtrLect[cO];
    if (cP > 0) aP = new PtrLect[cP];
    if (cE > 0) aE = new PtrLect[cE];

    s = 0;
    while (s < numSalas) {
        SalaUCI& sala = salas[s];
        int i = 0;
        while (i < sala.numMaquinas) {
            Maquina& M = sala.maquinas[i];
            int j = 0;
            while (j < M.numMediciones) {
                Medicion& md = M.mediciones[j];
                if (md.idPaciente[0] == target) {
                    int k = 0;
                    while (k < md.numLecturas) {
                        Lectura& L = md.lecturas[k];
                        PtrLect p; p.md = &md; p.L = &L;
                        if      (L.tipoSensor=='T') { aT[nT] = p; nT = nT + 1; }
                        else if (L.tipoSensor=='O') { aO[nO] = p; nO = nO + 1; }
                        else if (L.tipoSensor=='P') { aP[nP] = p; nP = nP + 1; }
                        else if (L.tipoSensor=='E') { aE[nE] = p; nE = nE + 1; }
                        k = k + 1;
                    }
                }
                j = j + 1;
            }
            i = i + 1;
        }
        s = s + 1;
    }

    if (nT>1) sort_ptrs(aT,nT);
    if (nO>1) sort_ptrs(aO,nO);
    if (nP>1) sort_ptrs(aP,nP);
    if (nE>1) sort_ptrs(aE,nE);

    if (nT>0) {
        out << "== Temperatura (°C) ==\n";
        double minV=0.0,maxV=0.0,sum=0.0; 
        int first=1; 
        int i=0;
        while (i<nT) {
            const Medicion* md = aT[i].md; 
            const Lectura* L = aT[i].L;
            int anom = esAnomalaLectura(*L, configuraciones, numConfigs);
            const char* estado = "NORMAL";
            if (anom) estado = "ANOMALA";
            double v = L->valor[0];
            if (first){ minV=v; maxV=v; first=0; }
            else { if(v<minV) minV=v; if(v>maxV) maxV=v; }
            sum += v;
            out << md->fechaHora << " | " << v << " °C | " << estado << "\n";
            i = i + 1;
        }
        double prom = 0.0;
        if (nT>0) prom = sum / (double)nT;
        out << "Min: " << minV << " | Max: " << maxV << " | Prom: " << prom << "\n\n";
    }

    if (nP>0) {
        out << "== Presión arterial (mmHg) ==\n";
        double minA=0.0,maxA=0.0,sumA=0.0; 
        int first=1; 
        int i=0;
        while (i<nP) {
            const Medicion* md = aP[i].md; 
            const Lectura* L = aP[i].L;
            double ps = L->valor[0];
            double pd = L->valor[1];
            double avg = 0.5*(ps + pd);
            int anom = esAnomalaLectura(*L, configuraciones, numConfigs);
            const char* estado = "NORMAL";
            if (anom) estado = "ANOMALA";
            if (first){ minA=avg; maxA=avg; first=0; }
            else { if(avg<minA) minA=avg; if(avg>maxA) maxA=avg; }
            sumA += avg;
            out << md->fechaHora << " | PS: " << ps << " | PD: " << pd
                << " | Prom: " << avg << " mmHg | " << estado << "\n";
            i = i + 1;
        }
        double prom = 0.0;
        if (nP>0) prom = sumA / (double)nP;
        out << "MinProm: " << minA << " | MaxProm: " << maxA << " | PromProm: " << prom << "\n\n";
    }

    if (nO>0) {
        out << "== Oximetría (%) ==\n";
        double minV=0.0,maxV=0.0,sum=0.0; 
        int first=1; 
        int i=0;
        while (i<nO) {
            const Medicion* md = aO[i].md; 
            const Lectura* L = aO[i].L;
            int anom = esAnomalaLectura(*L, configuraciones, numConfigs);
            const char* estado = "NORMAL";
            if (anom) estado = "ANOMALA";
            double v = L->valor[0];
            if (first){ minV=v; maxV=v; first=0; }
            else { if(v<minV) minV=v; if(v>maxV) maxV=v; }
            sum += v;
            out << md->fechaHora << " | " << v << " % | " << estado << "\n";
            i = i + 1;
        }
        double prom = 0.0;
        if (nO>0) prom = sum / (double)nO;
        out << "Min: " << minV << " | Max: " << maxV << " | Prom: " << prom << "\n\n";
    }

    if (nE>0) {
        out << "== ECG (mV) ==\n";
        double minV=0.0,maxV=0.0,sum=0.0; 
        int first=1; 
        int i=0;
        while (i<nE) {
            const Medicion* md = aE[i].md; 
            const Lectura* L = aE[i].L;
            double v = L->valor[0];
            if (first){ minV=v; maxV=v; first=0; }
            else { if(v<minV) minV=v; if(v>maxV) maxV=v; }
            sum += v;
            out << md->fechaHora << " | " << v << " mV\n";
            i = i + 1;
        }
        double prom = 0.0;
        if (nE>0) prom = sum / (double)nE;
        out << "Min: " << minV << " | Max: " << maxV << " | Prom: " << prom << "\n\n";
    }

    out.close();

    if (aT) delete[] aT;
    if (aO) delete[] aO;
    if (aP) delete[] aP;
    if (aE) delete[] aE;
}