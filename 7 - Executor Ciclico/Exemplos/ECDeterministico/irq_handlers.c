/**
 * ------------------------------------------------------------
 *  Arquivo: irq_handlers.c
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Este arquivo implementa o handler de interrupção do
 *      canal DMA 0, utilizado para leitura do sensor interno
 *      de temperatura via ADC do Raspberry Pi Pico W.
 *
 *      A função 'dma_handler_temp()' é responsável por
 *      capturar a interrupção do DMA, limpar o status e
 *      sinalizar a finalização da transferência de dados
 *      via flag global 'dma_temp_done'.
 *
 *  Relacionamento:
 *      - Este handler é registrado em 'setup.c' usando:
 *            irq_set_exclusive_handler(DMA_IRQ_0, dma_handler_temp);
 *      - A flag 'dma_temp_done' é usada em 'tarefa1_temp.c'
 *        para controlar o fluxo da aquisição via DMA.
 *
 *  
 *  Data: 11/05/2025
 * ------------------------------------------------------------
 */

#include "hardware/dma.h"
#include "irq_handlers.h"

// Flag global que sinaliza a conclusão da transferência DMA
volatile bool dma_temp_done = false;

/**
 * @brief Handler de interrupção do canal DMA 0.
 *
 * Esta função é chamada automaticamente quando o canal 0 do DMA
 * completa a transferência das amostras do ADC para o buffer.
 * Ela limpa a flag da interrupção e ativa o sinalizador
 * 'dma_temp_done' para que o laço principal saiba que pode
 * prosseguir.
 */
void dma_handler_temp() {
    dma_hw->ints0 = 1u << 0;   // Limpa a interrupção do canal 0
    dma_temp_done = true;     // Sinaliza conclusão para o executor
}