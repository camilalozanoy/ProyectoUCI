#include <fstream>
#include <cstring>
#include "utiles.h"

static bool es_linea_valida(const char* linea) {
    const char* p = linea;
    while (*p == ' ' || *p == '\t' || *p == '\r') ++p;
    if (*p == '\0' || *p == '\n' || *p == '#') return false;
    if (p[0] == '\r' && p[1] == '\n') return false;
    return true;
}

int contarLineasValidas(const char* archivo, char /*delimitador*/) {
    std::ifstream file(archivo);
    if (!file) return 0;

    char linea[1024];
    int count = 0;

    while (file.getline(linea, sizeof(linea))) {
        size_t len = std::strlen(linea);
        if (len > 0 && (linea[len-1] == '\r' || linea[len-1] == '\n')) {
            linea[len-1] = '\0';
            len--;
        }
        if (es_linea_valida(linea)) count++;
    }

    file.close();
    return count;
}
