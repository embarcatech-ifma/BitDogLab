/*
 * Projeto: Leitura da Temperatura Interna com DMA no Raspberry Pi Pico W
 * Autor: Diego Sampaio
 *
 * Objetivo:
 * Este programa realiza a leitura da temperatura interna do microcontrolador RP2040
 * utilizando o ADC (Conversor Analógico-Digital) e transfere os dados para um buffer
 * utilizando DMA (Acesso Direto à Memória), permitindo leituras rápidas e eficientes
 * sem sobrecarregar a CPU. O programa calcula a média de 100 amostras e imprime a
 * temperatura resultante em °C a cada segundo.
 */

 #include <stdio.h>
 #include "pico/stdlib.h"     // Funções básicas do SDK Pico
 #include "hardware/adc.h"    // Controle do ADC interno
 #include "hardware/dma.h"    // Controle do DMA
 
 #define NUM_SAMPLES 100      // Número de amostras por ciclo de leitura
 
 uint16_t adc_buffer[NUM_SAMPLES]; // Buffer para armazenar as amostras do ADC
 
 // Converte o valor bruto do ADC (12 bits) para temperatura em graus Celsius
 float convert_to_celsius(uint16_t raw) {
     const float conversion_factor = 3.3f / (1 << 12); // Fator de conversão para 3.3V e 12 bits
     float voltage = raw * conversion_factor;          // Converte valor para tensão
     return 27.0f - (voltage - 0.706f) / 0.001721f;     // Fórmula do datasheet do RP2040
 }
 
 int main() {
     stdio_init_all();   // Inicializa a saída padrão (USB serial)
     sleep_ms(2000);     // Aguarda 2 segundos para estabilizar o terminal serial
 
     // Inicializa o ADC e habilita o sensor de temperatura interno
     adc_init();
     adc_set_temp_sensor_enabled(true);
     adc_select_input(4); // Canal 4 é o sensor de temperatura interna do RP2040
 
     // Configura o canal DMA para receber dados do ADC
     int dma_chan = dma_claim_unused_channel(true); // Requisita um canal DMA disponível
     dma_channel_config cfg = dma_channel_get_default_config(dma_chan); // Obtem configuração padrão
 
     // Configurações do canal DMA
     channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);  // Cada leitura é de 16 bits
     channel_config_set_read_increment(&cfg, false);            // Endereço fixo (registrador ADC FIFO)
     channel_config_set_write_increment(&cfg, true);            // Incrementa para armazenar em adc_buffer[]
     channel_config_set_dreq(&cfg, DREQ_ADC);                   // Dispara automaticamente com dados do ADC
 
     while (true) {
         // Garante que o FIFO esteja vazio antes de começar
         adc_fifo_drain();
 
         // Configura o ADC para colocar dados no FIFO
         adc_run(false);  // Desliga ADC temporariamente
         adc_fifo_setup(
             true,   // Envia dados para o FIFO
             true,   // Habilita DMA para o FIFO
             1,      // Gatilho a cada amostra
             false,
             false
         );
         adc_run(true);   // Liga ADC para começar a amostrar
 
         // Inicia a transferência DMA: do FIFO ADC para adc_buffer
         dma_channel_configure(
             dma_chan,
             &cfg,
             adc_buffer,             // Endereço de destino na RAM
             &adc_hw->fifo,          // Endereço de origem (registrador FIFO do ADC)
             NUM_SAMPLES,            // Número de transferências (amostras)
             true                    // Inicia imediatamente
         );
 
         // Aguarda até que a transferência DMA seja concluída
         dma_channel_wait_for_finish_blocking(dma_chan);
 
         // Desliga o ADC após capturar os dados
         adc_run(false);
 
         // Calcula a média das temperaturas lidas
         float sum = 0.0f;
         for (int i = 0; i < NUM_SAMPLES; i++) {
             sum += convert_to_celsius(adc_buffer[i]); // Converte cada valor para °C e soma
         }
 
         float avg_temp = sum / NUM_SAMPLES; // Temperatura média em °C
         printf("Temperatura média: %.2f °C\n", avg_temp); // Imprime no terminal
 
         sleep_ms(1000); // Aguarda 1 segundo antes da próxima leitura
     }
 }
 
