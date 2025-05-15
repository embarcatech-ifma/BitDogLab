#include "LabNeoPixel/neopixel_driver.h"
#include "pico/stdlib.h"
#include "testes_cores.h"
#include "LabNeoPixel/efeitos.h"

typedef struct {
    uint8_t r, g, b;
} CorRGB;

void preencher_matriz_com_cores(void) {
    const CorRGB cores[] = {
        {COR_MIN, COR_APAGA, COR_APAGA},  // Vermelho
        {COR_APAGA, COR_MIN, COR_APAGA},  // Verde
        {COR_APAGA, COR_APAGA, COR_MIN},  // Azul
        {COR_MIN, COR_MIN, COR_APAGA},    // Amarelo
        {COR_APAGA, COR_MIN, COR_MIN},    // Ciano
        {COR_MIN, COR_APAGA, COR_MIN},    // Magenta
        {COR_MIN, COR_MIN, COR_MIN}       // Branco suave
    };

    const uint8_t total_cores = sizeof(cores) / sizeof(cores[0]);

    for (uint8_t i = 0; i < total_cores; ++i) {
        npSetAll(cores[i].r, cores[i].g, cores[i].b);
        npWrite();
        sleep_ms(700);
    }
}

void testar_fileiras_colunas(void) {
    // Fileiras de cima para baixo em vermelho suave
    for (uint8_t y = 0; y < NUM_LINHAS; y++) {
        npClear();
        acenderFileira(y, COR_MIN, COR_APAGA, COR_APAGA); // vermelho
        npWrite();
        sleep_ms(250);
    }

    sleep_ms(500);

    // Colunas da esquerda para a direita em azul suave
    for (uint8_t x = 0; x < NUM_COLUNAS; x++) {
        npClear();
        acenderColuna(x, COR_APAGA, COR_APAGA, COR_MIN); // azul
        npWrite();
        sleep_ms(250);
    }

    sleep_ms(500);
}
