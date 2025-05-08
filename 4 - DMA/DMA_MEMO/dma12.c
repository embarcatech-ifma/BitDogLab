/*
 * Projeto: Cópia de Palavra Digitada via DMA com Limpeza de Buffer
 * 
 * Adaptado de exemplos oficiais disponíveis no GitHub do Raspberry Pi Pico SDK.
 *
 * Objetivo:
 * Este programa permite ao usuário digitar uma palavra pelo terminal.
 * A palavra é copiada automaticamente via DMA do buffer `src` para `dst`.
 * Antes de cada nova entrada, os buffers são limpos (zerados), evitando resíduos.
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"

#define MAX_LEN 100

char src[MAX_LEN];  // Buffer de entrada
char dst[MAX_LEN];  // Buffer de destino

int main() {
    stdio_init_all();
    sleep_ms(2000);

    while (true) {
        // Limpa os buffers antes de cada nova leitura
        memset(src, 0, sizeof(src));
        memset(dst, 0, sizeof(dst));

        printf("Digite uma palavra (até 99 caracteres):\n");
        sleep_ms(2000);
        // Lê a nova palavra digitada
        scanf("%99s", src);

        // Calcula o tamanho da palavra incluindo o '\0'
        int len = strlen(src) + 1;

        // Solicita canal DMA livre
        int chan = dma_claim_unused_channel(true);

        // Configura o canal DMA
        dma_channel_config cfg = dma_channel_get_default_config(chan);
        channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
        channel_config_set_read_increment(&cfg, true);
        channel_config_set_write_increment(&cfg, true);

        // Inicia a transferência via DMA
        dma_channel_configure(
            chan, &cfg,
            dst, src,
            len,
            true
        );

        // Aguarda o fim da transferência
        dma_channel_wait_for_finish_blocking(chan);

        // Libera o canal DMA para uso futuro
        dma_channel_unclaim(chan);

        // Mostra a palavra copiada
        printf("Palavra copiada via DMA: %s\n\n", dst);
    }
}
