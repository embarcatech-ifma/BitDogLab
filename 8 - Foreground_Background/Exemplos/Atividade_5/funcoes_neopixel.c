#include "funcoes_neopixel.h"
#include "ws2818b.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

uint ordem[] = {
    4, 5, 14, 15, 24,
    3, 6, 13, 16, 23,
    2, 7, 12, 17, 22,
    1, 8, 11, 18, 21,
    0, 9, 10, 19, 20
};

npLED_t leds[LED_COUNT];

PIO np_pio;
uint sm;

void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }

    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < LED_COUNT) {
        leds[index].R = r;
        leds[index].G = g;
        leds[index].B = b;
    }
}

void npSetAll(uint8_t r, uint8_t g, uint8_t b) {
    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = r;
        leds[i].G = g;
        leds[i].B = b;
    }
}


void acenderFileira(uint linha, uint8_t r, uint8_t g, uint8_t b, uint colunas) {
    uint inicio = linha * colunas;
    for (uint i = 0; i < colunas; ++i) {
        uint index = inicio + i;
        if (index < LED_COUNT) {
            npSetLED(index, r, g, b);
        }
    }
}

void acender_coluna(uint8_t coluna, uint8_t r, uint8_t g, uint8_t b) {
    for (int linha = 0; linha < NUM_LINHAS; linha++) {
        uint index = linha * NUM_COLUNAS + coluna;
        leds[index].R = r;
        leds[index].G = g;
        leds[index].B = b;
    }
    npWrite();
}


void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}  

void npAcendeLED(uint index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < LED_COUNT) {
        npSetLED(index, r, g, b);
        npWrite(); // envia a nova configuração para a fita
    }
}

// Inicialize o gerador de números aleatórios uma vez no início do programa
void inicializar_aleatorio() {
    srand(time(NULL));
}

// Gera um número aleatório entre min e max (inclusive)
int numero_aleatorio(int min, int max) {
    return rand() % (max - min + 1) + min;
}