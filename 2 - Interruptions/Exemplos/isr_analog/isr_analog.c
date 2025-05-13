#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#define JOY_Y_ADC_CHANNEL 1  // ADC1 = GPIO27 = Eixo Y do joystick
#define LED_VERMELHO 12      // LED da BitDogLab (ânodo comum)

#define INTERVALO_MS 50      // Tempo entre amostragens
#define LIMITE_ACIONAMENTO 3500  // Valor mínimo para acionar (joystick empurrado para cima)

volatile bool acionado = false;

// Callback do timer periódico
bool leitura_joystick_callback(struct repeating_timer *t) {
    adc_select_input(JOY_Y_ADC_CHANNEL);
    uint16_t valor = adc_read();

    if (valor > LIMITE_ACIONAMENTO) {
        if (!acionado) {
            acionado = true;
            printf("Acionado! Valor do ADC: %d\n", valor);
            gpio_put(LED_VERMELHO, 0); // Acende (ânodo comum)
        }
    } else {
        if (acionado) {
            acionado = false;
            gpio_put(LED_VERMELHO, 1); // Apaga
            printf("Desativado. Valor do ADC: %d\n", valor);
        }
    }

    return true; // Continua chamando o callback
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("🎮 Sistema iniciado. Aguarde...\n");

    // Inicializa LED
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 1); // LED apagado inicialmente

    // Inicializa ADC
    adc_init();
    adc_gpio_init(27); // GPIO27 = ADC1

    // Adiciona timer periódico
    struct repeating_timer timer;
    add_repeating_timer_ms(INTERVALO_MS, leitura_joystick_callback, NULL, &timer);

    // Loop principal faz nada, tudo controlado pelo timer
    while (true) {
        tight_loop_contents();
    }

    return 0;
}
