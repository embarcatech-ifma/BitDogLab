#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/irq.h"

#define JOY_Y_ADC_CHANNEL 1   // ADC1 = GPIO27
#define JOY_X_ADC_CHANNEL 0   // GPIO26
#define LED_VERMELHO 12       // LED ânodo comum
#define LIMITE_ACIONAMENTO 3500

volatile bool acionado = false;

// Função de tratamento da interrupção do ADC
void adc_irq_handler() {
    while (adc_fifo_get_level() >= 2) { // Garantir par X,Y

        uint16_t valor1 = adc_fifo_get();
        uint16_t valor2 = adc_fifo_get();
    
        // Sabemos que o round-robin está entre canal 0 (X) e canal 1 (Y)
        uint16_t valor_x = valor1;
        uint16_t valor_y = valor2;

        // Processamento dos valores
        if (valor_y > LIMITE_ACIONAMENTO) {
            if (!acionado) {
                acionado = true;
                gpio_put(LED_VERMELHO, 0);
                printf("Acionado! X: %d, Y: %d\n", valor_x, valor_y);
            }
        } else {
            if (acionado) {
                acionado = false;
                gpio_put(LED_VERMELHO, 1);
                printf("Desativado. X: %d, Y: %d\n", valor_x, valor_y);
            }
        }
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("🎮 Sistema iniciado. Aguarde...\n");

    // Inicializa o LED
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 1); // LED apagado inicialmente

    // Inicializa o ADC
    adc_init();
    adc_gpio_init(26); // X
    adc_gpio_init(27); // GPIO27 = ADC1
    adc_select_input(JOY_X_ADC_CHANNEL); // Primeiro canal
    adc_select_input(JOY_Y_ADC_CHANNEL);
    adc_set_round_robin((1 << JOY_X_ADC_CHANNEL) | (1 << JOY_Y_ADC_CHANNEL));

    // Configura FIFO do ADC
    adc_fifo_setup(
        true,    // Enabling the FIFO
        true,    // Enable DMA data request (não vamos usar aqui, mas o pico SDK recomenda deixar true)
        1,       // DREQ (número de amostras antes de gerar IRQ) = 1
        false,   // Trigger only on threshold (não)
        false    // No error bit
    );

    // Configura a interrupção
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);
    irq_set_enabled(ADC_IRQ_FIFO, true);
    adc_irq_set_enabled(true);

    // Começa a primeira conversão
    adc_fifo_drain(); // Limpa qualquer dado antigo
    adc_run(true);
    
    while (true) {
        // valor = adc_fifo_get(); 
        // printf("Valor do ADC: %d\n", valor);
        sleep_ms(50); // Pequeno delay para não gerar muitas leituras (pode ser ajustado)
    }
}
