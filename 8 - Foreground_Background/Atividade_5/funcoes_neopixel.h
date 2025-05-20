#ifndef FUNCOES_NEOPIXEL_H
#define FUNCOES_NEOPIXEL_H

#include <stdint.h>
#include "hardware/pio.h"
#include <time.h>
#include <stdlib.h>
#include "hardware/adc.h"
#include "pico/types.h"

#define LED_COUNT 25
#define LED_PIN 7
#define NUM_COLUNAS 5
#define NUM_LINHAS 5

typedef struct {
    uint8_t G, R, B;
} npLED_t;

extern uint ordem[];
extern npLED_t leds[];

extern PIO np_pio;
extern uint sm;
extern volatile uint index_neo;

void npInit(uint pin);
void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b);
void npSetAll(uint8_t r, uint8_t g, uint8_t b);
void acenderFileira(uint linha, uint8_t r, uint8_t g, uint8_t b, uint colunas);
void acender_coluna(uint8_t coluna, uint8_t r, uint8_t g, uint8_t b);
void npClear();
void npWrite();
void npAcendeLED(uint index, uint8_t r, uint8_t g, uint8_t b);
void inicializar_aleatorio();
int numero_aleatorio(int min, int max);

#endif
