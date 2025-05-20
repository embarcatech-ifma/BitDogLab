#include "funcao_atividade_2.h"

int main() {
    stdio_init_all();
    adc_init();

    // Inicializa LEDs
    for (int i = 0; i < NUM_BOTOES; i++) {
        inicializar_pino(LEDS[i], GPIO_OUT, false, false);
        gpio_put(LEDS[i], 0); // Garante que o LED começa apagado
    }

    // Inicializa botões com pull-up
    for (int i = 0; i < NUM_BOTOES; i++) {
        inicializar_pino(BOTOES[i], GPIO_IN, true, false);
    }

    // 1) Registra o callback global para GPIO nesse core
    gpio_set_irq_callback(gpio_callback);

    // 2) Habilita o vetor de interrupção de GPIO
    irq_set_enabled(IO_IRQ_BANK0, true);

    // 3) Escolhe quais eventos disparam no pino (borda de descida, ao apertar)
    gpio_set_irq_enabled(BOTAO_A, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true);


    while (true) 
    {
        __wfi();  // Aguarda interrupção
        for (int i = 0; i < NUM_BOTOES; i++) 
            if (botoes_pressionados[i]) 
            {
                botoes_pressionados[i] = false;        
                estado_leds[i] = !estado_leds[i];
                gpio_put(LEDS[i], estado_leds[i]);
                sleep_ms(DELAY_MS);  // Feedback visual
            }
    }
    return 0;
}