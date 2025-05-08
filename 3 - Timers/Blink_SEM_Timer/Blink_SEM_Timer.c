#include <stdio.h>
#include "pico/stdlib.h"
#include <time.h>

#define LED_VERDE 11
#define LED_AZUL  12

int main() {
    stdio_init_all();

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, 0);

    // ponto de partida em ticks
    clock_t inicial = clock();

    while (true) {
        // pisca azul
        gpio_put(LED_AZUL, 1);
        sleep_ms(100);
        gpio_put(LED_AZUL, 0);
        sleep_ms(100);

        // captura ticks atuais
        clock_t atual = clock();
        // converte diferença para milissegundos
        clock_t delta_ms = (double)(atual - inicial) * 1000.0 / CLOCKS_PER_SEC;
        printf("delta_ms: %d\n", delta_ms);

        if (delta_ms >= 500.0) {
            // evento verde
            gpio_put(LED_VERDE, 1);
            sleep_ms(2000);
            gpio_put(LED_VERDE, 0);
            // reinicia referência em ticks
            inicial = atual;
        }
    }
}
