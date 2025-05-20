#include "funcao_atividade_.h"
#include "funcoes_neopixel.h"

typedef struct {
    uint8_t r, g, b;
} CorRGB;

CorRGB cores[] = {
    {255, 0, 0}, {255, 64, 0}, {255, 128, 0}, {255, 192, 0},
    {255, 255, 0}, {192, 255, 0}, {128, 255, 0}, {0, 255, 0},
    {0, 255, 128}, {0, 255, 255}, {0, 128, 255}, {0, 0, 255},
    {128, 0, 255}, {255, 0, 255}, {255, 0, 128}, {255, 255, 255}
};

const size_t TOTAL_CORES = sizeof(cores) / sizeof(CorRGB);
volatile uint index_neo = 0;

int main() {
    npInit(LED_PIN);
    npClear();
    npWrite();

    stdio_init_all();

    // Inicializa LEDs
    for (int i = 0; i < NUM_BOTOES; i++) {
        inicializar_pino(LEDS[i], GPIO_OUT, false, false);
        gpio_put(LEDS[i], 0);
        estado_leds[i] = false;
    }

    // Inicializa botões
    for (int i = 0; i < NUM_BOTOES; i++) {
        inicializar_pino(BOTOES[i], GPIO_IN, true, false);
    }

    // Inicia Core 1 para tratar os LEDs
    multicore_launch_core1(tratar_eventos_leds);

// Aguarda o núcleo 1 sinalizar que está pronto
while (!core1_pronto);

    // Núcleo 0 configura e monitora interrupções
    gpio_set_irq_callback(gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    for (int i = 0; i < NUM_BOTOES; i++) {
        gpio_set_irq_enabled(BOTOES[i], GPIO_IRQ_EDGE_FALL, true);
    }

    // Núcleo 0 entra em espera
    while (true) {
        __wfi(); // Fica em modo de espera, interrupções cuidam de tudo
    }
}