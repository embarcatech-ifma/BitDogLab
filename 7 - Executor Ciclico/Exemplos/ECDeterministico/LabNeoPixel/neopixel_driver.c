#include "neopixel_driver.h"
#include "ws2818b.pio.h"

npLED_t leds[LED_COUNT];
PIO np_pio;
int sm;

void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = 0; // Usar SM 0 fixamente
    pio_sm_claim(np_pio, sm);
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
    npClear();
}

void npWrite(void) {
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
}

void npWriteComBrilho(float brilho) {
    for (uint i = 0; i < LED_COUNT; ++i) {
        uint8_t r = leds[i].R * brilho;
        uint8_t g = leds[i].G * brilho;
        uint8_t b = leds[i].B * brilho;
        pio_sm_put_blocking(np_pio, sm, g);
        pio_sm_put_blocking(np_pio, sm, r);
        pio_sm_put_blocking(np_pio, sm, b);
    }
}

void npSetLED(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < LED_COUNT) {
        leds[index].R = r;
        leds[index].G = g;
        leds[index].B = b;
    }
}

void npSetAll(uint8_t r, uint8_t g, uint8_t b) {
    for (uint i = 0; i < LED_COUNT; ++i) {
        npSetLED(i, r, g, b);
    }
}

void npClear(void) {
    npSetAll(0, 0, 0);
}

void liberar_maquina_pio(PIO pio, uint sm_id) {
    if (sm_id < 4) {
        pio_sm_set_enabled(pio, sm_id, false);
        pio_sm_unclaim(pio, sm_id);
    }
}

uint getLEDIndex(uint x, uint y) {
    if (x >= NUM_COLUNAS || y >= NUM_LINHAS) return 0;
    uint linha_fisica = NUM_LINHAS - 1 - y;
    uint base = linha_fisica * NUM_COLUNAS;
    return (linha_fisica % 2 == 0) ? base + (NUM_COLUNAS - 1 - x) : base + x;
}
