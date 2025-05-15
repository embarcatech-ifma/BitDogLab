/**
 * ------------------------------------------------------------
 *  Arquivo: tarefa4_controla_neopixel.h
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Interface da Tarefa 4: controle visual da matriz NeoPixel
 *      conforme a tendência térmica calculada na Tarefa 3.
 *
 *      A função exposta permite colorir todos os LEDs com:
 *         - Azul     → tendência caindo
 *         - Verde    → tendência estável
 *         - Vermelho → tendência subindo
 *
 *      Essa interface deve ser incluída no `main.c` para executar
 *      a tarefa após a análise da tendência.
 *
 *  
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#ifndef TAREFA4_CONTROLA_NEOPIXEL_H
#define TAREFA4_CONTROLA_NEOPIXEL_H

#include "tarefa3_tendencia.h"  // para o tipo tendencia_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define a cor da matriz NeoPixel conforme a tendência térmica.
 *
 * @param t Valor da tendência detectada: SUBINDO, ESTÁVEL ou CAINDO
 */
void tarefa4_matriz_cor_por_tendencia(tendencia_t t);

#ifdef __cplusplus
}
#endif

#endif  // TAREFA4_CONTROLA_NEOPIXEL_H