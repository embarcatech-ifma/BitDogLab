#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BOTAO_A 5
#define BOTAO_B 6
#define LED     11

void isr_botoes(uint gpio, uint32_t events) {
    if (gpio == BOTAO_A) {
        gpio_put(LED, 1); // Acende o LED
        printf("Liga\n");
    } else if (gpio == BOTAO_B) {
        gpio_put(LED, 0); // Apaga o LED
        printf("Desliga\n");
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &isr_botoes);
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        tight_loop_contents(); // Espera interrupções
    }
}
