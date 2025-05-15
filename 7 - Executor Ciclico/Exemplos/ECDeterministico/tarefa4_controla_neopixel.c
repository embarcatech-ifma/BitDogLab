/**
 * ------------------------------------------------------------
 *  Arquivo: tarefa4_controla_neopixel.c
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Este módulo implementa a Tarefa 4 do executor cíclico:
 *      controle da cor de toda a matriz NeoPixel com base
 *      na tendência térmica detectada pela Tarefa 3.
 *
 *      A lógica é simples e visualmente intuitiva:
 *         - Tendência SUBINDO  → matriz toda VERMELHA
 *         - Tendência ESTÁVEL → matriz toda VERDE
 *         - Tendência CAINDO  → matriz toda AZUL
 *
 *      As cores são aplicadas a todos os LEDs simultaneamente,
 *      utilizando a função npSetAll() do driver de NeoPixels.
 *
 *  Relacionamento:
 *      - Depende de `tarefa3_tendencia.h` para o enum `tendencia_t`
 *      - Usa `neopixel_driver.h` para acionar os LEDs
 *      - Requer definições simbólicas de cores (ex: `COR_AZUL`)
 *
 *  
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#include "neopixel_driver.h"
#include "tarefa3_tendencia.h"
#include "testes_cores.h"  // contém COR_AZUL, COR_VERDE, COR_VERMELHO

/**
 * @brief Define a cor de todos os LEDs da matriz de acordo com a tendência.
 *
 * @param t Tendência térmica detectada (subindo, caindo, estável)
 */
void tarefa4_matriz_cor_por_tendencia(tendencia_t t) {
    switch (t) {
        case TENDENCIA_CAINDO:
            npSetAll(COR_AZUL);     // Azul
            break;
        case TENDENCIA_ESTÁVEL:
            npSetAll(COR_VERDE);    // Verde
            break;
        case TENDENCIA_SUBINDO:
            npSetAll(COR_VERMELHO); // Vermelho
            break;
    }

    npWrite();  // Atualiza fisicamente a matriz
}
