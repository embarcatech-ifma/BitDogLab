/**
 * ------------------------------------------------------------
 *  Arquivo: tarefa2_display.c
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Tarefa 2 corrigida: exibe no display OLED duas linhas:
 *
 *         TEMP: 32.4 C
 *         TEND: ESTAVEL
 *
 *      Ambas centralizadas horizontalmente, usando fonte padrão.
 *
 *  
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include "display_utils.h"
#include "tarefa2_display.h"
#include "tarefa3_tendencia.h"

extern uint8_t ssd[];
extern struct render_area area;

void tarefa2_exibir_oled(float temperatura, tendencia_t tendencia) {
    ssd1306_clear_display(ssd);

    // === Linha 1: temperatura média ===
    char linha_temp[20];
    snprintf(linha_temp, sizeof(linha_temp), "TEMP: %.1f C", temperatura);
    int x1 = (128 - strlen(linha_temp) * 6) / 2;
    ssd1306_draw_string(ssd, x1, 16, linha_temp);  // Y = 16 px (linha 2)

    // === Linha 2: tendência ===
    char linha_tend[30];
    snprintf(linha_tend, sizeof(linha_tend), "TEND: %s", tendencia_para_texto(tendencia));
    int x2 = (128 - strlen(linha_tend) * 6) / 2;
    ssd1306_draw_string(ssd, x2, 32, linha_tend);  // Y = 32 px (linha 4)

    render_on_display(ssd, &area);
}

