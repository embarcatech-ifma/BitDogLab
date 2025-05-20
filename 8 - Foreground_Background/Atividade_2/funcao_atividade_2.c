#include "funcao_atividade_2.h"

volatile bool botoes_pressionados[NUM_BOTOES] = {false, false, false};
volatile uint64_t tempo_ultimo_acionamento[NUM_BOTOES] = {0, 0, 0};
bool estado_leds[NUM_BOTOES] = {false, false, false};
const uint BOTOES[NUM_BOTOES] = {BOTAO_A, BOTAO_B, BOTAO_JOYSTICK};
const uint LEDS[NUM_BOTOES]   = {LED_VERMELHO, LED_AZUL, LED_VERDE};


// ======= IMPLEMENTAÇÃO DAS FUNÇÕES =======
// Callback de interrupção com debounce
void gpio_callback(uint gpio, uint32_t events) {
    uint64_t agora = time_us_64();  // Captura o tempo atual

    for (int i = 0; i < NUM_BOTOES; i++) {
        if (gpio == BOTOES[i] && (events & GPIO_IRQ_EDGE_FALL)) {
            if (agora - tempo_ultimo_acionamento[i] > DEBOUNCE_MS * 1000) {
                tempo_ultimo_acionamento[i] = agora;
                botoes_pressionados[i] = true;
            }
        }
    }
}

// Inicializa um pino como entrada ou saída
void inicializar_pino(uint pino, uint direcao, bool pull_up, bool pull_down) {
    if (direcao != GPIO_IN && direcao != GPIO_OUT) {
        return;
    }

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
