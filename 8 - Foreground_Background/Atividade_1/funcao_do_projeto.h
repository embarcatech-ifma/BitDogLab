#ifndef FUNCAO_DO_PROJETO_H
#define FUNCAO_DO_PROJETO_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_VERMELHO 13   // Pino para o LED vermelho
#define LED_VERDE 11 // Pino para o LED verde
#define LED_AZUL 12  // Pino para o LED azul

#define BOTAO_A 5    // GPIO conectado ao Botão A
#define BOTAO_B 6    // GPIO conectado ao Botão B
#define BOTAO_C 22  // GPIO conectado ao Joystick, botão de pressionar o joystick.

#define DELAY_MS 500 // Define um tempo entre cores (em milissegundos)

void inicializar_pino(uint pino, uint direcao, bool pull_up, bool pull_down);
void atuar_no_led(uint pino_, uint delay_);

#endif