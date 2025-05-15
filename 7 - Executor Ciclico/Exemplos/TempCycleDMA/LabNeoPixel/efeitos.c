#include "LabNeoPixel/neopixel_driver.h"
#include "LabNeoPixel/efeitos.h"
#include "pico/stdlib.h"
#include "testes_cores.h"
#include <stdlib.h> 


// Acende todos os LEDs de uma linha
void acenderFileira(uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    for (uint x = 0; x < NUM_COLUNAS; x++) {
        uint index = getLEDIndex(x, y);
        npSetLED(index, r, g, b);
    }
    npWrite();
}

// Acende todos os LEDs de uma coluna
void acenderColuna(uint8_t x, uint8_t r, uint8_t g, uint8_t b) {
    for (uint y = 0; y < NUM_LINHAS; y++) {
        uint index = getLEDIndex(x, y);
        npSetLED(index, r, g, b);
    }
    npWrite();
}

// Preenche a matriz em espiral do canto superior esquerdo ao centro
void efeitoEspiral(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    const uint8_t ordem_espiral[25][2] = {
        {0,0},{1,0},{2,0},{3,0},{4,0},
        {4,1},{4,2},{4,3},{4,4},
        {3,4},{2,4},{1,4},{0,4},
        {0,3},{0,2},{0,1},
        {1,1},{2,1},{3,1},
        {3,2},{3,3},
        {2,3},{1,3},
        {1,2},{2,2}
    };

    npClear();
    for (uint i = 0; i < 25; ++i) {
        uint x = ordem_espiral[i][0];
        uint y = ordem_espiral[i][1];
        uint index = getLEDIndex(x, y);
        npSetLED(index, r, g, b);
        npWrite();
        sleep_ms(delay_ms);
    }
}

// Efeito de onda vertical com brilho suavizado por linha
void efeitoOndaVertical(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    for (int fase = 0; fase < NUM_LINHAS + 3; ++fase) {
        npClear();
        for (int y = 0; y < NUM_LINHAS; ++y) {
            float intensidade = 1.0f - 0.25f * abs(fase - y);
            if (intensidade < 0) intensidade = 0;

            for (int x = 0; x < NUM_COLUNAS; ++x) {
                uint index = getLEDIndex(x, y);
                npSetLED(index, r * intensidade, g * intensidade, b * intensidade);
            }
        }
        npWrite();
        sleep_ms(delay_ms);
    }
}

// Preenche a matriz em espiral do canto superior esquerdo ao centro, inversa.
void efeitoEspiralInversa(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    const uint8_t ordem_espiral[25][2] = {
        {2,2},{1,2},{1,3},{2,3},{3,3},
        {3,2},{3,1},{2,1},{1,1},
        {0,1},{0,2},{0,3},{0,4},
        {1,4},{2,4},{3,4},
        {4,4},{4,3},{4,2},
        {4,1},{4,0},
        {3,0},{2,0},
        {1,0},{0,0}
    };

    npClear();
    for (uint i = 0; i < 25; ++i) {
        uint x = ordem_espiral[i][0];
        uint y = ordem_espiral[i][1];
        uint index = getLEDIndex(x, y);
        npSetLED(index, r, g, b);
        npWrite();
        sleep_ms(delay_ms);
    }
}


//Onda com efeito vertical brilho
void efeitoOndaVerticalBrilho(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    for (uint8_t passo = 0; passo < NUM_LINHAS; ++passo) {
        npClear();

        for (uint8_t y = 0; y <= passo; ++y) {
            // Brilho progressivo proporcional à linha atual
            float brilho = ((float)(y + 1)) / NUM_LINHAS;

            for (uint8_t x = 0; x < NUM_COLUNAS; ++x) {
                uint index = getLEDIndex(x, y);
                npSetLED(index, r * brilho, g * brilho, b * brilho);
            }
        }

        npWrite();
        sleep_ms(delay_ms);
    }
}


void efeitoFileirasColoridas(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    for (uint8_t y = 0; y < NUM_LINHAS; ++y) {
        npClear();

        float brilho = ((float)(y + 1)) / NUM_LINHAS;

        acenderFileira(y, r * brilho, g * brilho, b * brilho);

        npWrite();
        sleep_ms(delay_ms);
    }
}

void efeitoFileirasColoridasReverso(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    for (int8_t y = NUM_LINHAS - 1; y >= 0; --y) {
        npClear();

        float brilho = ((float)(NUM_LINHAS - y)) / NUM_LINHAS;

        acenderFileira(y, r * brilho, g * brilho, b * brilho);

        npWrite();
        sleep_ms(delay_ms);
    }
}


void efeitoColunasColoridas(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    for (uint8_t x = 0; x < NUM_COLUNAS; ++x) {
        npClear();

        float brilho = ((float)(x + 1)) / NUM_COLUNAS;

        acenderColuna(x, r * brilho, g * brilho, b * brilho);

        npWrite();
        sleep_ms(delay_ms);
    }
}

void efeitoColunasColoridasReverso(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    for (int8_t x = NUM_COLUNAS - 1; x >= 0; --x) {
        npClear();

        float brilho = ((float)(NUM_COLUNAS - x)) / NUM_COLUNAS;

        acenderColuna(x, r * brilho, g * brilho, b * brilho);

        npWrite();
        sleep_ms(delay_ms);
    }
}
