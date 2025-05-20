#ifndef FUNCAO_ATIVIDADE_4_H
#define FUNCAO_ATIVIDADE_4_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "pico/multicore.h"
#include "hardware/sync.h"

#define BOTAO_A 5
#define BOTAO_B 6
#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12

#define NUM_BOTOES 2
#define TAM_FILA 25

#define ACAO_INSERIR 1
#define ACAO_REMOVER 2

extern const uint BOTOES[NUM_BOTOES];
extern const uint LEDS[3];
extern volatile bool core1_pronto;

void gpio_callback(uint gpio, uint32_t events);
void inicializar_pino(uint pino, uint direcao, bool pull_up, bool pull_down);
void tratar_eventos_leds();
void set_rgb(uint r, uint g, uint b);
void imprimir_fila();

#endif