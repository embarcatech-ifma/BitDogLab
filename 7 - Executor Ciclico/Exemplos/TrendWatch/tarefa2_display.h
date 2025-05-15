/**
 * ------------------------------------------------------------
 *  Arquivo: tarefa2_display.h
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Interface da Tarefa 2: exibição no display OLED.
 *      Exibe a temperatura média em fonte grande e a
 *      tendência térmica detectada pela Tarefa 3.
 *
 *  
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#ifndef TAREFA2_DISPLAY_H
#define TAREFA2_DISPLAY_H

#include "tarefa3_tendencia.h"  // necessário para tipo tendencia_t

/**
 * @brief Exibe no OLED a temperatura média e a tendência térmica.
 *
 * @param temperatura Valor da temperatura média atual
 * @param tendencia Resultado da análise de tendência (subindo, caindo, estável)
 */
void tarefa2_exibir_oled(float temperatura, tendencia_t tendencia);

#endif  // TAREFA2_DISPLAY_H
