#include "funcao_atividade_3.h"

const uint BOTOES[NUM_BOTOES] = {BOTAO_A, BOTAO_B, BOTAO_JOYSTICK};
const uint LEDS[NUM_BOTOES]   = {LED_VERMELHO, LED_AZUL, LED_VERDE};

volatile bool eventos_pendentes[NUM_BOTOES] = {false, false, false};
bool estado_leds[NUM_BOTOES] = {false, false, false};

// Callback simples
void gpio_callback(uint gpio, uint32_t events) {
    for (int i = 0; i < NUM_BOTOES; i++) {
        if (gpio == BOTOES[i] && (events & GPIO_IRQ_EDGE_FALL)) {
            // Desativa interrupção até tratarmos no loop
            gpio_set_irq_enabled(BOTOES[i], GPIO_IRQ_EDGE_FALL, false);
            eventos_pendentes[i] = true;
        }
    }
}

void inicializar_pino(uint pino, uint direcao, bool pull_up, bool pull_down) {
    gpio_init(pino);
    gpio_set_dir(pino, direcao);

    if (direcao == GPIO_IN) {
        if (pull_up) {
            gpio_pull_up(pino);
        } else if (pull_down) {
            gpio_pull_down(pino);
        } else {
            gpio_disable_pulls(pino);
        }
    }
}
