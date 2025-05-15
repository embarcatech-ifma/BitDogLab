#ifndef NEOPIXEL_DRIVER_H
#define NEOPIXEL_DRIVER_H

#include <stdint.h>
#include "hardware/pio.h"

#define LED_COUNT 25
#define LED_PIN 7
#define NUM_COLUNAS 5
#define NUM_LINHAS 5
#define COR_APAGA   0
#define COR_MIN     64
#define COR_INTER   128
#define COR_ALTA    192


typedef struct {
    uint8_t G, R, B;
} npLED_t;

extern npLED_t leds[LED_COUNT];
extern PIO np_pio;
extern int sm;

void npInit(uint pin);
void npWrite(void);
void npWriteComBrilho(float brilho);
void npSetLED(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void npSetAll(uint8_t r, uint8_t g, uint8_t b);
void npClear(void);
void liberar_maquina_pio(PIO pio, uint sm);
uint getLEDIndex(uint x, uint y);

#endif