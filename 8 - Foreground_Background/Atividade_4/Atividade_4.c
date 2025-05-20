#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "pico/multicore.h"
#include "funcao_atividade_4.h"

int main() {
    stdio_init_all();

    // Inicializa LEDs RGB
    for (int i = 0; i < 3; i++) {
        inicializar_pino(LEDS[i], GPIO_OUT, false, false);
        gpio_put(LEDS[i], 0);
    }

    // Inicializa botões
    for (int i = 0; i < NUM_BOTOES; i++) {
        inicializar_pino(BOTOES[i], GPIO_IN, true, false);
    }

    multicore_launch_core1(tratar_eventos_leds);

    while (!core1_pronto);

    // Configura interrupções
    gpio_set_irq_callback(gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    for (int i = 0; i < NUM_BOTOES; i++) {
        gpio_set_irq_enabled(BOTOES[i], GPIO_IRQ_EDGE_FALL, true);
    }

    while (true) {
        __wfi();
    }
}
