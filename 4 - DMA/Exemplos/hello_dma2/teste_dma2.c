/**
 * Projeto: Cópia de Dados com DMA no Raspberry Pi Pico
 * Autor: Adaptado do GitHub do Raspberry Pi Pico SDK.
 * Baseado em exemplo oficial disponível no GitHub do Raspberry Pi Pico SDK.
 * 
 * Objetivo:
 * Este programa demonstra o uso do DMA (Direct Memory Access) no Raspberry Pi Pico
 * para copiar dados de um buffer de origem (`src`) para um buffer de destino (`dst`)
 * sem utilizar a CPU diretamente durante a cópia. Após a cópia, os dados são exibidos
 * continuamente no terminal. É um exemplo simples e eficiente para aprender a usar DMA
 * em aplicações embarcadas com o RP2040.
 */

 #include <stdio.h>
 #include "pico/stdlib.h"     // Funções padrão do SDK Pico
 #include "hardware/dma.h"    // Controle do periférico DMA
 #include <string.h>          // Para strlen()
 
 // Texto de origem a ser copiado para outro buffer via DMA
 const char src[] = "Hello, world! EmbarcaTech";
 
 // Buffer de destino com o mesmo tamanho do buffer de origem
 char dst[count_of(src)];
 
 int main() {
     stdio_init_all(); // Inicializa a interface de saída padrão (ex: USB serial)
 
     // Solicita um canal DMA livre (ou entra em pânico se não houver nenhum)
     int chan = dma_claim_unused_channel(true);
 
     // Configura o canal DMA:
     // - Transferência de 8 bits (1 byte)
     // - Incrementa os endereços de leitura e escrita
     dma_channel_config c = dma_channel_get_default_config(chan);
     channel_config_set_transfer_data_size(&c, DMA_SIZE_8);      // Cada transferência terá 8 bits
     channel_config_set_read_increment(&c, true);                // Avança o ponteiro de leitura (src)
     channel_config_set_write_increment(&c, true);               // Avança o ponteiro de escrita (dst)
 
     // Inicia a transferência de dados do src para o dst
     dma_channel_configure(
         chan,                // Canal DMA utilizado
         &c,                  // Configurações aplicadas
         dst,                 // Endereço de destino (RAM)
         src,                 // Endereço de origem (RAM)
         count_of(src),       // Quantidade de bytes a copiar
         true                 // Inicia a transferência imediatamente
     );
 
     // Aguarda até que a transferência DMA seja concluída
     dma_channel_wait_for_finish_blocking(chan);
 
     // A partir deste ponto, o conteúdo de src foi copiado para dst via DMA
 
     while(true){
         // Imprime o conteúdo do buffer dst repetidamente a cada 500 ms
         for(int i = 0; i < strlen(dst); i++){
             printf("%c", dst[i]);
         }
         printf("\n");
         sleep_ms(500);
     }
 }
 