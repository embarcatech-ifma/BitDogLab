#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Define o pino que receberá os eventos externos (pulso de descida EDGE_FALL)
#define BOTAO 5

volatile uint32_t contador = 0;

// Função de callback chamada na interrupção
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BOTAO && (events & GPIO_IRQ_EDGE_FALL)) {
        contador++;
    }
}

int main() {
    // Inicializa a comunicação serial para monitorar o contador
    stdio_init_all();

    // Configura o pino como entrada
    gpio_init(BOTAO);
    gpio_set_dir(BOTAO, GPIO_IN);
    gpio_pull_up(BOTAO); // Resistor de PULL_UP

    // Configura a interrupção para detectar borda de descida
    gpio_set_irq_enabled_with_callback(BOTAO, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true) {
        printf("Contador de eventos: %u\n", contador);
        sleep_ms(1000); // Atualiza a cada segundo
    }

    return 0;
}
