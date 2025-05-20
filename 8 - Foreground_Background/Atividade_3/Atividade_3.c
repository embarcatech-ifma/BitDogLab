#include "funcao_atividade_3.h"

int main() {
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

    // Configura interrupções
    gpio_set_irq_callback(gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    for (int i = 0; i < NUM_BOTOES; i++) {
        gpio_set_irq_enabled(BOTOES[i], GPIO_IRQ_EDGE_FALL, true);
    }

    // Loop principal
    while (true) {
        for (int i = 0; i < NUM_BOTOES; i++) {
            if (eventos_pendentes[i]) {
                eventos_pendentes[i] = false;

                // Aguarda tempo de debounce
                sleep_ms(DEBOUNCE_MS);

                // Verifica se o botão ainda está pressionado
                if (!gpio_get(BOTOES[i])) {
                    // Troca estado do LED
                    estado_leds[i] = !estado_leds[i];
                    gpio_put(LEDS[i], estado_leds[i]);

                    sleep_ms(DELAY_MS);
                }

                // Reabilita a interrupção no botão após tratamento
                gpio_set_irq_enabled(BOTOES[i], GPIO_IRQ_EDGE_FALL, true);
            }
        }

        __wfi();  // Economia de energia enquanto espera
    }
}