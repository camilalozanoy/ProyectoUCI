# Sistemas de Monitoreo Biomédico (UCI) — Proyecto Práctico C++

Procesa archivos **.bsf** (binarios) de sensores biomédicos (UCI) y archivos de texto de configuración/pacientes para:
- Leer y estructurar lecturas por **Sala → Máquina → Medición → Lectura**.
- Detectar **anomalías** en T, O y P (ECG se analiza aparte según sprint).
- Generar **reporte por paciente** (`mediciones_paciente_<id>.txt`) con orden cronológico y estadísticas básicas.
- Exportar **datos procesados** a `datos_procesados.txt`.

> **Restricciones cumplidas**: sin librerías externas; manejo con **arreglos dinámicos** (sin `std::vector`), `string` solo para tokenizar.

---

## Estructura del repositorio

