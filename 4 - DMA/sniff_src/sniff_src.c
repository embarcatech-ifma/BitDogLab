/*
 * Projeto: Verificação de Integridade com CRC32 usando DMA Sniffer no Raspberry Pi Pico
 * 
 * Adaptado de exemplo oficial disponível no GitHub do Raspberry Pi Pico SDK.
 *
 * Objetivo:
 * Este programa demonstra o uso do recurso "sniffer" do DMA do RP2040 para calcular
 * automaticamente o CRC32 de um buffer de dados. A técnica é útil para validar a 
 * integridade de blocos de memória de forma rápida e sem uso intensivo da CPU.
 *
 * O DMA é configurado para ler um buffer contendo dados e um CRC32 previamente
 * calculado, transferindo os dados para um destino fictício (`dummy_dst`), ao mesmo
 * tempo em que calcula o CRC internamente com o sniffer.
 * Ao final, o valor calculado é comparado com o CRC embutido no buffer.
 */

 #include <stdio.h>
 #include <string.h>
 #include "pico/stdlib.h"   // Funções padrão do Pico SDK
 #include "hardware/dma.h"  // Controle do DMA
 
 #define CRC32_INIT                  ((uint32_t)-1l)   // Valor inicial do CRC32 (todos os bits em 1)
 
 #define DATA_TO_CHECK_LEN           9   // Tamanho dos dados a serem verificados
 #define CRC32_LEN                   4   // Tamanho do CRC32 (4 bytes)
 #define TOTAL_LEN                   (DATA_TO_CHECK_LEN + CRC32_LEN) // Total = dados + CRC
 
 // Buffer com 9 bytes de dados + 4 bytes reservados para armazenar o CRC
 static uint8_t src[TOTAL_LEN] = {
     0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,  // ASCII: "123456789"
     0x00, 0x00, 0x00, 0x00                                // espaço reservado para CRC
 };
 
 // Destino fictício — o DMA precisa de um destino, mesmo que não seja usado
 static uint8_t dummy_dst[1];
 
 // Função que calcula CRC32 por software (para gerar o valor inicial do buffer)
 // Usa polinômio padrão em modo "reversed"
 static uint32_t soft_crc32_block(uint32_t crc, uint8_t *bytp, uint32_t length) {
     while(length--) {
         uint32_t byte32 = (uint32_t)*bytp++;
 
         for (uint8_t bit = 8; bit; bit--, byte32 >>= 1) {
             crc = (crc >> 1) ^ (((crc ^ byte32) & 1ul) ? 0xEDB88320ul : 0ul);
         }
     }
     return crc;
 }
 
 int main() {
     uint32_t crc_res;
 
     stdio_init_all();  // Inicializa a saída padrão (USB serial)
 
     // Calcula o CRC32 via software sobre os 9 bytes de dados
     crc_res = soft_crc32_block(CRC32_INIT, src, DATA_TO_CHECK_LEN);
 
     // Armazena o CRC calculado nos últimos 4 bytes do buffer
     *((uint32_t *)&src[DATA_TO_CHECK_LEN]) = crc_res;
 
     // Mostra o conteúdo completo do buffer que será verificado
     printf("Buffer a ser analisado via DMA Sniffer: ");
     for (int i = 0; i < TOTAL_LEN; i++) {
         printf("0x%02x ", src[i]);
     }
     printf("\n");
 
     // [Opcional] Descomente para simular erro na verificação
     // src[0]++; // modifica o primeiro byte, corrompendo os dados
 
     // Solicita um canal DMA disponível
     int chan = dma_claim_unused_channel(true);
 
     // Configura o canal DMA:
     // - Transfere 8 bits por operação
     // - Incrementa o endereço de leitura (src)
     // - NÃO incrementa o endereço de escrita (sempre escreve no dummy_dst[0])
     dma_channel_config c = dma_channel_get_default_config(chan);
     channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
     channel_config_set_read_increment(&c, true);
     channel_config_set_write_increment(&c, false);
 
     // Configura o sniffer para calcular CRC32 no modo reversed
     channel_config_set_sniff_enable(&c, true);
     dma_sniffer_set_data_accumulator(CRC32_INIT);                         // Valor inicial
     dma_sniffer_set_output_reverse_enabled(true);                         // Ativa bit reverse
     dma_sniffer_enable(chan, DMA_SNIFF_CTRL_CALC_VALUE_CRC32R, true);     // Modo CRC32 reversed
 
     // Inicia a transferência do buffer src para o dummy_dst
     dma_channel_configure(
         chan, &c,
         dummy_dst,     // Endereço de escrita (fixo e fictício)
         src,           // Endereço de leitura
         TOTAL_LEN,     // Número total de bytes (dados + CRC)
         true           // Iniciar imediatamente
     );
 
     // Aguarda o fim da transferência
     dma_channel_wait_for_finish_blocking(chan);
 
     // Obtém o resultado do sniffer (CRC acumulado)
     uint32_t sniffed_crc = dma_sniffer_get_data_accumulator();
 
     // Verifica o resultado continuamente
     while(true){
         printf("DMA Sniffer terminou a análise do buffer (%d bytes). Resultado: 0x%x\n", TOTAL_LEN, sniffed_crc);
 
         if (sniffed_crc == 0ul) {
             printf("CRC32 verificado com sucesso ✅\n");
         } else {
             printf("❌ ERRO: CRC32 não confere!\n");
         }
 
         sleep_ms(1000);
     }
 }
 