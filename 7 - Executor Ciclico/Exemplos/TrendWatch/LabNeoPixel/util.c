#include "util.h"
#include <stdlib.h>
#include <time.h>

void inicializar_aleatorio(void) {
    srand(time(NULL));
}

int numero_aleatorio(int min, int max) {
    return rand() % (max - min + 1) + min;
}

float numero_aleatorio_0a1(void) {
    return (float)rand() / (float)RAND_MAX;
}