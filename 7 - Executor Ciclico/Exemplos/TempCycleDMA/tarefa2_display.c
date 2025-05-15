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
    render_on_display(ssd, &area);  // Garante que a limpeza seja visível
    sleep_ms(20);                   // Pequeno delay opcional

    char* linha1 = "Temperatura";
    char* linha2 = "Media";
    char linha3[30];

    snprintf(linha3, sizeof(linha3), "TEMP: %s", tendencia_para_texto(tendencia));

    // Fonte padrão: 6 px por caractere, altura: 8 px
    int x1 = (128 - strlen(linha1) * 6) / 2;
    int x2 = (128 - strlen(linha2) * 6) / 2;
    int x3 = (128 - strlen(linha3) * 6) / 2;

    // Y = linha × altura da fonte (8 px padrão)
    ssd1306_draw_string(ssd, x1, 0, linha1);    // Linha 0 (Y=0)
    // Linha 1 = em branco (Y=8)
    ssd1306_draw_string(ssd, x2, 16, linha2);   // Linha 2 (Y=16)
    // Linha 3 = em branco (Y=24)

    // Fonte grande começa abaixo: Y=32 px
    mostrar_valor_grande(ssd, temperatura, 32);

    ssd1306_draw_string(ssd, 0, 56, linha3);  // Y = 32 

    render_on_display(ssd, &area);
}

