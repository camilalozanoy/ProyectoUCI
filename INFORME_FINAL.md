# INFORME_FINAL — Estructuras y Funciones Implementadas

1) Estructuras utilizadas

# Definidas en `structs.h`
```cpp
struct Umbral {
    char tipoSensor[6];   // "T", "O", "P_SIS", "P_DIA", "E"
    double umbralMin;
    double umbralMax;
};

struct Configuracion {
    Umbral umbrales[5];
    int tam = 0;
};

struct Paciente {
    char id[11];
    char tipoDocumento[5];
    char documento[20];
    char nombres[50];
    char apellidos[50];
    char fechaNacimiento[11];
    char telefono[15];
    char email[50];
    char tipoSangre[5];
    char entidadSalud[50];
    char medicinaPrepagada[50];
};

struct ListaPacientes{
    Paciente* pacientes = nullptr;
    int tam = 0;
};

struct Lectura {
    char tipoSensor;      // 'T', 'P', 'O', 'E'
    double valor[2];      // T/O/E usan valor[0]; P usa valor[0]=PS y valor[1]=PD
};

struct Medicion {
    char idPaciente[11];
    char fechaHora[24];   // "DD/MM/AAAA HH:MM:ss.sss"
    int numLecturas;
    Lectura* lecturas;
};

struct Maquina {
    char idMaquina;
    int numMediciones;
    Medicion* mediciones;
};

struct SalaUCI {
    char idSala;
    int numMaquinas;
    Maquina* maquinas;
};

2) Funciones implementadas (por archivo)
A) carga.h
Umbral* separarConfiguracion(const char* linea);


Parsea una línea "TIPO,MIN,MAX" y construye un Umbral (en heap) con tipoSensor, umbralMin, umbralMax.

Configuracion* cargarConfiguracion(const char* archivo);


Lee configuracion.txt, llama a separarConfiguracion por cada línea y copia el resultado en Configuracion::umbrales, incrementando tam.

B) lecturaBSF.h / lecturaBSF.cpp
int byteAIntPositivo(char b);


Convierte un char a entero en el rango [0..255] (útil para diagnosticar bytes no ASCII).

void leerArchivoBSF(const char* archivo, SalaUCI*& salas, int& numSalas);


Abre y lee el .bsf campo a campo y reserva arreglos dinámicos:

Sala: [idSala:1][numMaquinas:1]

Máquina: [id:1][numMediciones:4] → new Medicion[numMediciones]

Medición: [idPaciente:1][fecha:24][numLecturas:4]

Lectura: [tipo:1] + payload (T/E/O: double; P: float ps + float pd)

C) anomalias.h / anomalias.cpp
bool detectarAnomalia(const Lectura& lectura,
                      const Umbral* configuraciones, int numConfigs);


Determina si una lectura es anómala:

T/O: compara valor[0] con [min,max].

P: calcula avg = (PS + PD) / 2 y lo compara con el promedio de umbrales de P_SIS y P_DIA.

E: retorna false (no se clasifica en esta versión).

int buscarUmbral(const Umbral* lista, int total,
                 const char* nombreBuscado, double& minimo, double& maximo);


Busca por clave ("T", "O", "P_SIS", "P_DIA", "E") comparando carácter a carácter (hasta 6). Si encuentra, escribe minimo/maximo y retorna 1.

D) reportes.h / reportes.cpp
int parseFecha(const char* s, int& D,int& M,int& Y,int& h,int& m,int& sec,int& ms);


Extrae los componentes de "DD/MM/AAAA HH:MM:ss.sss".

int cmpFecha(const char* a, const char* b);


Compara dos fechas completas y retorna -1, 0 o 1.

void ordenarMedicionesPorFecha(Medicion* mediciones, int numMediciones);


Ordena un arreglo de Medicion por fechaHora.

int getUmbral(const Umbral* cfg, int n, const char* tipo, double& umin, double& umax);


Obtiene [umin,umax] para la clave de sensor; 1 si existe.

int esAnomalaLectura(const Lectura& L, const Umbral* cfg, int n);


Indica si L es anómala según cfg (T/O/P; E no aplica).

struct PtrLect { const Medicion* md; const Lectura* L; };
void sort_ptrs(PtrLect* a, int n);


Ordena un arreglo de punteros a lecturas por la fecha de su Medicion.

void generarReportePaciente(const char* idPaciente,
                            SalaUCI* salas, int numSalas,
                            const Umbral* configuraciones, int numConfigs);


Genera mediciones_paciente_<id>.txt: agrupa por tipo (T/P/O/E), ordena por fecha y escribe estado (cuando aplica) y min/max/prom por sección.

E) estadisticas.h / estadisticas.cpp
void calcularEstadisticas(const SalaUCI* salas, int numSalas, const char* idPaciente,
                          double& minV, double& maxV, double& promV, char tipoSensor);


Recorre todas las lecturas del paciente y tipo indicados y calcula mínimo, máximo y promedio.
En P usa avg = (PS + PD) / 2.

F) exportarDatos.h / exportarDatos.cpp
void exportarDatosProcesados(const SalaUCI* salas, int numSalas,
                             const Umbral* configuraciones, int numConfigs);


Recorre toda la jerarquía y genera datos_procesados.txt indicando NORMAL/ANÓMALO para T/O/P en cada lectura.

G) utiles.h / utiles.cpp
int contarLineasValidas(const char* archivo, char delimitador);


Abre un archivo de texto y cuenta líneas no vacías ni que empiecen por #.
(Internamente, utiles.cpp usa un helper estático es_linea_valida(...) para descartar comentarios y líneas vacías.)


3) Uso de IA — Ejemplos de prompts utilizados

1) **Leer un archivo binario (.bsf) desde cero**
> *Prompt:*  
> “Chat estoy haciendo un proyecto en C++ y no sé cómo leer un archivo binario, me explicas paso a paso y de manera sencilla cómo abrirlo y leer los datos, ten en cuenta que quiero ir guardando las cosas en arreglos dinámicos y que no puedo usar `vector`.”

2) **¿Mi función para leer `.bsf` está bien? (revisión rápida)**
> *Prompt:*  
> “Tengo esta función para leer un **.bsf** y armar en memoria Sala → Máquina → Medición → Lectura, te voy a subir el código en un mensaje, indicame qué está mal?”

3) **Entender la estructura del `.bsf`**
> *Prompt:*  
> “Me explicas de manera muy sencilla y fácil de entender cómo viene la información en el .bsf: primero sala, luego máquinas, luego mediciones, luego lecturas, y qué tamaños tienen; supon que no sé programar y necesito una guía rápida para no confundirme al leerlo.”

4) **Lógica para hacer el reporte por paciente**
> *Prompt:*  
> “Necesito crear un reporte por paciente, que debería hacer? teniendo en mente que quiero filtrar por id, separar por tipo de sensor (T, P, O, E), ordenar por fecha y al final calcular mínimo, máximo y promedio, dame una ruta (paso a paso) fácil de entender (sin `vector`)?”

5) **Estadísticas (mínimo, máximo, promedio)**
> *Prompt:*  
> “Necesito calcular min, max y promedio de las lecturas de un paciente por tipo de sensor, como lo hago recorriendo todas las máquinas y mediciones?”

6) **Exportar todo a un `.txt` legible**
> *Prompt:*  
> “Quiero **guardar todo** en un archivo de texto (`datos_procesados.txt`) de forma bonita: sala, máquina, fecha, tipo de sensor, valor y si es NORMAL o ANÓMALO (en T/O/P). ¿Cómo lo organizo para que sea fácil de leer? Dame un ejemplo de cómo quedaría.”

7) **README para GitHub (en español, súper claro)**
> *Prompt:*  
> “Necesito que me ayudes a escribir un README fácil de entender, necesito que tenga: requisitos, cómo compilar detalla muy bien, cómo ejecutar, qué archivos necesito (configuración y `.bsf`), y qué archivos se generan (reporte y exportado).”

8) **Verificar que cumplo las reglas del proyecto**
> *Prompt:*  
> “Necesito que me ayudes a revisar si estoy cumpliendo con las restricciones, tengo prohibido usar `vector` ni listas, solo arreglos con `new[]`. También que los datos de texto los guarde en `char[]`.”

9) **Guía rápida para subir todo a GitHub**
> *Prompt:*  
> “Necesito que me des un paso a paso en GitHub para crear el repo, agregar mis archivos, crear el README y el informe.”
