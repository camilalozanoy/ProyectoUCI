# Sistemas de Monitoreo Biomédico (UCI) — Proyecto Práctico C++

Procesa archivos **.bsf** (binarios) de sensores biomédicos (UCI) y archivos de texto de configuración/pacientes para:
- Leer y estructurar lecturas por **Sala → Máquina → Medición → Lectura**.
- Detectar **anomalías** en T, O y P (ECG se analiza aparte según sprint).
- Generar **reporte por paciente** (`mediciones_paciente_<id>.txt`) con orden cronológico y estadísticas básicas.
- Exportar **datos procesados** a `datos_procesados.txt`.

# Compilar y Ejecutar

## Compilar (paso a paso)

1. Abre una terminal **dentro de la carpeta del proyecto** (donde está `main.cpp`).
2. Ejecuta **uno** de estos comandos según tu sistema:

   **Linux / macOS / Raspberry Pi**
   ```bash
   g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o uci_biomonitor
Ejecutar (paso a paso)
1.Deja el ejecutable en la misma carpeta junto con:
configuracion.txt
Tu archivo .bsf (por ejemplo: patient_readings_simulation_small.bsf)
2.Ejecuta el programa.

## En el menú, sigue este flujo recomendado
1. Cargar el .bsf
Opción 5 → escribe el nombre del archivo .bsf → Enter.
Se cargan en memoria Sala → Máquinas → Mediciones → Lecturas.

2. Generar reporte de un paciente
Opción 2 → escribe el ID del paciente y Enter.
Se crea mediciones_paciente_<id>.txt con las lecturas ordenadas por fecha, agrupadas por sensor y con estado NORMAL/ANÓMALA cuando aplica, más min/max/prom por tipo.

3. Exportar todos los datos procesados
Opción 4 → se crea datos_procesados.txt con el recorrido completo (Sala/Máquina/Medición/Lectura) y estado NORMAL/ANÓMALO en T/O/P.
ECG (E) solo se reporta (no se clasifica en esta versión).

4. Calcular estadísticas puntuales
Opción 3 → escribe ID del paciente y tipo de sensor (T, P, O, E).
Muestra en pantalla mínimo, máximo y promedio.

5. Salir
Opción 6.

## Resumen 

Lee el binario .bsf y arma la estructura en memoria (Sala → Máquina → Medición → Lectura).

Detecta anomalías para T y O (fuera de [min,max]) y para P usando el promedio (PS+PD)/2 comparado con P_SIS/P_DIA.
(ECG E se reporta sin clasificar).

Genera mediciones_paciente_<id>.txt (orden cronológico, estado por lectura y estadísticas por sensor).

Exporta datos_procesados.txt con todo el contenido cargado y estado NORMAL/ANÓMALO (T/O/P).
