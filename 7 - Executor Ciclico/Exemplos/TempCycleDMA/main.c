/**
 * ------------------------------------------------------------
 *  Arquivo: main.c
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Ciclo principal do sistema embarcado, baseado em um
 *      executor cíclico com 3 tarefas principais:
 *
 *      Tarefa 1 - Leitura da temperatura via DMA (meio segundo)
 *      Tarefa 2 - Exibição da temperatura e tendência no OLED
 *      Tarefa 3 - Análise da tendência da temperatura
 *
 *      O sistema utiliza watchdog para segurança, terminal USB
 *      para monitoramento e display OLED para visualização direta.
 *
 *  
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include "setup.h"
#include "tarefa1_temp.h"
#include "tarefa2_display.h"
#include "tarefa3_tendencia.h"
#include "tarefa4_controla_neopixel.h"
#include "neopixel_driver.h"
#include "testes_cores.h"  
#include "pico/stdio_usb.h"

void tarefa_3();
void tarefa_5();
void tarefa_4();
void tarefa_2();
void tarefa_1();


float media;
tendencia_t t;
absolute_time_t ini_tarefa1, fim_tarefa1, ini_tarefa2, fim_tarefa2, ini_tarefa3, fim_tarefa3, ini_tarefa4, fim_tarefa4;

int main() {
    setup();  // Inicializações: ADC, DMA, interrupções, OLED, etc.

   // while (!stdio_usb_connected()) {
   //     sleep_ms(100);
   // }

    // Ativa o watchdog com timeout de 2 segundos
    //watchdog_enable(2000, 1);

    while (true) {
        //watchdog_update();  // Alimente o watchdog no início de cada ciclo

        tarefa_1();
        tarefa_5();
        tarefa_2();
        tarefa_3();
        tarefa_4();

        // --- Cálculo dos tempos de execução ---
        int64_t tempo1_us = absolute_time_diff_us(ini_tarefa1, fim_tarefa1);
        int64_t tempo2_us = absolute_time_diff_us(ini_tarefa2, fim_tarefa2);
        int64_t tempo3_us = absolute_time_diff_us(ini_tarefa3, fim_tarefa3);
        int64_t tempo4_us = absolute_time_diff_us(ini_tarefa4, fim_tarefa4);

        // --- Exibição no terminal ---
        printf("Temperatura: %.2f °C | T1: %.3fs | T2: %.3fs | T3: %.3fs | T4: %.3fs | Tendência: %s\n",
               media,
               tempo1_us / 1e6,
               tempo2_us / 1e6,
               tempo3_us / 1e6,
               tempo4_us / 1e6,
               tendencia_para_texto(t));

        sleep_ms(1000);  // Aguarda próximo ciclo
    }

    return 0;
}

/*******************************/
void tarefa_1()
{
// --- Tarefa 1: Leitura de temperatura via DMA ---
        ini_tarefa1 = get_absolute_time();
        media = tarefa1_obter_media_temp(&cfg_temp, DMA_TEMP_CHANNEL);
        fim_tarefa1 = get_absolute_time();
}
/*******************************/
void tarefa_2()
{
    // --- Tarefa 3: Análise da tendência térmica ---
        ini_tarefa3 = get_absolute_time();
        t = tarefa3_analisa_tendencia(media);
        fim_tarefa3 = get_absolute_time();
}
/*******************************/
void tarefa_3()
{
        // --- Tarefa 2: Exibição no OLED ---
        ini_tarefa2 = get_absolute_time();
        tarefa2_exibir_oled(media, t);
        fim_tarefa2 = get_absolute_time();
}
/*******************************/
void tarefa_4()
{
// --- Tarefa 4: Cor da matriz NeoPixel por tendência ---
        absolute_time_t ini_tarefa4 = get_absolute_time();
        tarefa4_matriz_cor_por_tendencia(t);
        absolute_time_t fim_tarefa4 = get_absolute_time();
}
void tarefa_5()
{
// --- Tarefa 5: Extra ---
        while(media < 1)
        {
            npSetAll(COR_BRANCA);
            npWrite();
            sleep_ms(1000);
            npClear();
            npWrite();
            sleep_ms(1000);
        }
}