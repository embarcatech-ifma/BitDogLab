/*
 * Projeto: Transmissão Sequencial via UART usando Dois Canais DMA (Control Blocks)
 * 
 * Adaptado de exemplo oficial disponível no GitHub do Raspberry Pi Pico SDK.
 *
 * Objetivo:
 * Este programa demonstra como usar dois canais DMA no RP2040 para realizar
 * uma **transmissão sequencial de palavras via UART**, sem intervenção da CPU,
 * utilizando o recurso de **control blocks dinâmicos**.
 *
 * Um canal DMA (data channel) envia os dados para a UART.
 * Outro canal (control channel) programa dinamicamente o primeiro canal para 
 * transmitir a próxima palavra, criando uma **cadeia automática de transmissões**.
 */

 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/dma.h"
 #include "hardware/structs/uart.h"
 
 // Frases/palavras que serão transmitidas via UART uma a uma
 const char word0[] = "Transferring ";
 const char word1[] = "one ";
 const char word2[] = "word ";
 const char word3[] = "at ";
 const char word4[] = "a ";
 const char word5[] = "time.\n";
 
 // Blocos de controle DMA contendo o tamanho da palavra e o endereço do texto
 // Importante: a ordem dos campos é essencial para o funcionamento correto com alias 3
 const struct {
     uint32_t len;           // Quantidade de bytes a transferir
     const char *data;       // Endereço da string
 } control_blocks[] = {
     {count_of(word0) - 1, word0}, // remove '\0'
     {count_of(word1) - 1, word1},
     {count_of(word2) - 1, word2},
     {count_of(word3) - 1, word3},
     {count_of(word4) - 1, word4},
     {count_of(word5) - 1, word5},
     {0, NULL}  // Bloco nulo sinaliza fim da cadeia
 };
 
 int main() {
 #ifndef uart_default
 #warning dma/control_blocks example requires a UART
 #else
     stdio_init_all();
     puts("DMA control block example:");
 
     // Criação de dois canais DMA:
     // - ctrl_chan: programa blocos de controle para data_chan
     // - data_chan: envia bytes para a UART
     int ctrl_chan = dma_claim_unused_channel(true);
     int data_chan = dma_claim_unused_channel(true);
 
     // Configuração do canal de controle (ctrl_chan):
     // - Transfere dois valores de 32 bits (tamanho + endereço da string)
     // - Aponta para os registradores de controle do canal de dados (data_chan)
     dma_channel_config c = dma_channel_get_default_config(ctrl_chan);
     channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
     channel_config_set_read_increment(&c, true);
     channel_config_set_write_increment(&c, true);
     channel_config_set_ring(&c, true, 3); // endereço de escrita circular a cada 8 bytes
 
     dma_channel_configure(
         ctrl_chan,
         &c,
         &dma_hw->ch[data_chan].al3_transfer_count, // destino: registradores do canal de dados
         &control_blocks[0],                        // origem: blocos de controle
         2,                                         // transfere dois valores (len + data)
         false                                      // não inicia ainda
     );
 
     // Configuração do canal de dados (data_chan):
     // - Envia bytes para o FIFO da UART
     // - Controlado por DREQ (pacing automático)
     // - Ao terminar, encadeia para ctrl_chan, que programa o próximo bloco
     c = dma_channel_get_default_config(data_chan);
     channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
     channel_config_set_dreq(&c, uart_get_dreq(uart_default, true));
     channel_config_set_chain_to(&c, ctrl_chan);  // ao terminar, dispara ctrl_chan
     channel_config_set_irq_quiet(&c, true);      // evita gerar interrupção
 
     dma_channel_configure(
         data_chan,
         &c,
         &uart_get_hw(uart_default)->dr,  // destino: registrador da UART (envio)
         NULL,                            // origem será configurada dinamicamente
         0,                               // tamanho será configurado dinamicamente
         false                            // não inicia ainda
     );
 
     // Inicia a primeira programação DMA
     dma_start_channel_mask(1u << ctrl_chan);
 
     // Aguarda o fim de toda a cadeia de transmissões
     while (!(dma_hw->intr & (1u << data_chan))) {
         tight_loop_contents();  // espera ocupada
     }
 
     // Limpa o sinal de interrupção DMA manualmente
     dma_hw->ints0 = 1u << data_chan;
 
     puts("DMA finished.");
     sleep_ms(1000);
 #endif
 }
 