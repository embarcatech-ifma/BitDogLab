#include "funcao_atividade_4.h"

const uint BOTOES[NUM_BOTOES] = {BOTAO_A, BOTAO_B};
const uint LEDS[3] = {LED_VERMELHO, LED_VERDE, LED_AZUL};

volatile bool core1_pronto = false;

int fila[TAM_FILA];
int inicio = 0;
int fim = 0;
int quantidade = 0;
int contador = 0;

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BOTAO_A && (events & GPIO_IRQ_EDGE_FALL)) {
        multicore_fifo_push_blocking(ACAO_INSERIR);
    } else if (gpio == BOTAO_B && (events & GPIO_IRQ_EDGE_FALL)) {
        multicore_fifo_push_blocking(ACAO_REMOVER);
    }
}

void inicializar_pino(uint pino, uint direcao, bool pull_up, bool pull_down) {
    gpio_init(pino);
    gpio_set_dir(pino, direcao);
    if (direcao == GPIO_IN) {
        if (pull_up) gpio_pull_up(pino);
        else if (pull_down) gpio_pull_down(pino);
        else gpio_disable_pulls(pino);
    }
}

void set_rgb(uint r, uint g, uint b) {
    gpio_put(LED_VERMELHO, r > 0);
    gpio_put(LED_VERDE, g > 0);
    gpio_put(LED_AZUL, b > 0);
}

void tratar_eventos_leds() {
    core1_pronto = true;

    while (true) {
        uint32_t acao = multicore_fifo_pop_blocking();  // Espera ação

        if (acao == ACAO_INSERIR) {
            if (quantidade < TAM_FILA) {
                fila[fim] = contador++;
                fim = (fim + 1) % TAM_FILA;
                quantidade++;
                imprimir_fila();
            }
        } else if (acao == ACAO_REMOVER) {
            if (quantidade > 0) {
                int valor = fila[inicio];
                inicio = (inicio + 1) % TAM_FILA;
                quantidade--;
                imprimir_fila();
            }
        }

        // Atualiza LED RGB
        if (quantidade == 0) {
            set_rgb(0, 0, 255);  // Azul
        } else if (quantidade == TAM_FILA) {
            set_rgb(255, 0, 0);  // Vermelho
        } else {
            set_rgb(0, 255, 0);  // Verde
        }
    }
}

void imprimir_fila() {
    printf("Fila [tam=%d]: ", quantidade);
    int i = inicio;
    for (int c = 0; c < quantidade; c++) {
        printf("%d ", fila[i]);
        i = (i + 1) % TAM_FILA;
    }
    printf("\n");
}
