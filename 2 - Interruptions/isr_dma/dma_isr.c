#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#define LED_PIN 11
#define TAMANHO 16

uint8_t origem[TAMANHO]  = {1, 2, 3, 4, 5, 6, 7, 8,
                            9, 10, 11, 12, 13, 14, 15, 16};
uint8_t destino[TAMANHO];  // Buffer destino

int canal_dma;  // Canal de DMA
volatile bool dma_finalizado = false;  // Flag para sinalizar fim

// Interrupção DMA — leve, apenas sinaliza
void dma_isr() {
    dma_hw->ints0 = 1u << canal_dma;  // Limpa interrupção
    dma_finalizado = true;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);  // Tempo para conectar USB
    printf("🔁 Iniciando transferência DMA...\n");

    // Inicializa o LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Solicita canal DMA e configura
    canal_dma = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(canal_dma);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, true);

    // Configura transferência (sem iniciar ainda)
    dma_channel_configure(
        canal_dma,
        &config,
        destino,    // destino
        origem,     // origem
        TAMANHO,    // número de bytes
        false       // não inicia ainda
    );

    // Configura interrupção
    irq_set_exclusive_handler(DMA_IRQ_0, dma_isr);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_channel_set_irq0_enabled(canal_dma, true);

    // Inicia DMA com tudo pronto
    dma_channel_start(canal_dma);

    // Loop principal — monitora a flag
    while (true) {
        if (dma_finalizado) {
            dma_finalizado = false;

            // Pisca o LED
            for (int i = 0; i < 3; i++) {
                gpio_put(LED_PIN, 1);
                sleep_ms(150);
                gpio_put(LED_PIN, 0);
                sleep_ms(150);
            }

            // Exibe os dados transferidos
            printf("✅ Transferência DMA finalizada!\nConteúdo transferido: ");
            for (int i = 0; i < TAMANHO; i++) {
                printf("%d ", destino[i]);
            }
            printf("\n");
        }

        tight_loop_contents();  // Mantém o loop ativo
    }

    return 0;
}
