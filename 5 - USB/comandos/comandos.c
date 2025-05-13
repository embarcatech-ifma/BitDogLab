#include <stdio.h>
#include "pico/stdlib.h"

#define LED_A 11
#define LED_B 12
#define LED_C 13

void apaga_todos_leds() {
    gpio_put(LED_A, 0);
    gpio_put(LED_B, 0);
    gpio_put(LED_C, 0);
}

int main() {
    stdio_init_all();

    gpio_init(LED_A);
    gpio_set_dir(LED_A, true);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, true);
    gpio_init(LED_C);
    gpio_set_dir(LED_C, true);

    printf("Pronto para receber comandos!\n");

    while (true) {
        int ch = getchar_timeout_us(0); // não bloqueante

        if (ch != PICO_ERROR_TIMEOUT) {  // se recebeu algo
            char c = (char)ch;
            printf("Recebido: %c\n", c);

            apaga_todos_leds();  // apaga antes de acender o correto

            if (c == 'A' || c == 'a') {
                gpio_put(LED_A, 1);
            } else if (c == 'B' || c == 'b') {
                gpio_put(LED_B, 1);
            } else if (c == 'C' || c == 'c') {
                gpio_put(LED_C, 1);
            } else {
                printf("Caractere desconhecido.\n");
            }
        }
        sleep_ms(10);  // pequeno delay para evitar polling intenso
    }
}
