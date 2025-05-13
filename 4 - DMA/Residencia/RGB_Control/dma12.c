#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"

#define MAX_LEN 32

// Pinos RGB BitDog Lab
#define RED_PIN    13
#define GREEN_PIN  11
#define BLUE_PIN   12

char cmd[MAX_LEN];   // buffer para comando recebido
char resposta[64];   // buffer para resposta enviada por DMA

void configurar_gpio_rgb() {
    gpio_init(RED_PIN);   gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_init(GREEN_PIN); gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_init(BLUE_PIN);  gpio_set_dir(BLUE_PIN, GPIO_OUT);

    gpio_put(RED_PIN, 1);   // LED desligado (ânodo comum)
    gpio_put(GREEN_PIN, 1);
    gpio_put(BLUE_PIN, 1);
}

void controlar_led(const char* cor, const char* estado) {
    int nivel = (strcmp(estado, "on") == 0) ? 0 : 1;  // ânodo comum: 0 acende, 1 apaga

    if (strcmp(cor, "red") == 0) {
        gpio_put(RED_PIN, nivel);
    } else if (strcmp(cor, "green") == 0) {
        gpio_put(GREEN_PIN, nivel);
    } else if (strcmp(cor, "blue") == 0) {
        gpio_put(BLUE_PIN, nivel);
    }
}

void enviar_resposta_dma(const char* texto) {
    // DMA canal
    int dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, true);
    channel_config_set_write_increment(&cfg, false);

    channel_config_set_dreq(&cfg, DREQ_UART0_TX);  // UART0 padrão

    dma_channel_configure(
        dma_chan, &cfg,
        &uart0_hw->dr,       // Registrador de dados UART
        texto,
        strlen(texto),
        true
    );

    dma_channel_wait_for_finish_blocking(dma_chan);
    dma_channel_unclaim(dma_chan);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("🔧 Controle RGB via UART + DMA iniciado...\n");

    configurar_gpio_rgb();

    while (true) {
        memset(cmd, 0, sizeof(cmd));
        memset(resposta, 0, sizeof(resposta));
    
        printf("Digite um comando (ex: red on):\n");
    
        // Lê linha inteira
        char linha[MAX_LEN] = {0};
        fgets(linha, sizeof(linha), stdin);
    
        char cor[16], estado[16];
        if (sscanf(linha, "%15s %15s", cor, estado) == 2) {
            // Verifica se comando é válido
            if ((strcmp(cor, "red") == 0 || strcmp(cor, "green") == 0 || strcmp(cor, "blue") == 0) &&
                (strcmp(estado, "on") == 0 || strcmp(estado, "off") == 0)) {
    
                controlar_led(cor, estado);
                snprintf(resposta, sizeof(resposta), "✅ LED %s %s\n", cor, estado);
            } else {
                snprintf(resposta, sizeof(resposta), "❌ Comando inválido.\n");
            }
        } else {
            snprintf(resposta, sizeof(resposta), "❌ Entrada incompleta. Use: [cor] [on/off]\n");
        }
    
        enviar_resposta_dma(resposta);
    }

    return 0;
}
