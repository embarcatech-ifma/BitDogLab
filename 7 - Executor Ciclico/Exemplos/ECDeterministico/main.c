#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "hardware/timer.h"

#include "setup.h"
#include "tarefa1_temp.h"
#include "tarefa2_display.h"
#include "tarefa3_tendencia.h"
#include "tarefa4_controla_neopixel.h"
#include "neopixel_driver.h"
#include "testes_cores.h"
#include "pico/stdio_usb.h"

#define NUM_TAREFAS 5
#define SLOT_MS 1000

// Estados
volatile int tarefa_atual = 0;

// Tempo de execução por tarefa
absolute_time_t ini_tarefa1, fim_tarefa1;
absolute_time_t ini_tarefa2, fim_tarefa2;
absolute_time_t ini_tarefa3, fim_tarefa3;
absolute_time_t ini_tarefa4, fim_tarefa4;

// Variáveis de sistema
float media = 0;
tendencia_t t;

// Prototipagem
void tarefa_1();
void tarefa_2();
void tarefa_3();
void tarefa_4();
void tarefa_5();

bool timer_callback(repeating_timer_t *rt) {
    if(tarefa_atual == 0){
        printf("tarefa 1\n");
    }
}

int main() {
    setup();

    static repeating_timer_t timer;
    printf("Before timer\n");
    add_repeating_timer_ms(SLOT_MS, timer_callback, NULL, &timer);
    printf("after timer\n");
    while (true) {
        tight_loop_contents();
    }

    return 0;
}

/*******************************/
void tarefa_1() {
    ini_tarefa1 = get_absolute_time();
    media = tarefa1_obter_media_temp(&cfg_temp, DMA_TEMP_CHANNEL);
    fim_tarefa1 = get_absolute_time();
}

/*******************************/
void tarefa_2() {
    ini_tarefa3 = get_absolute_time();
    t = tarefa3_analisa_tendencia(media);
    fim_tarefa3 = get_absolute_time();
}

/*******************************/
void tarefa_3() {
    ini_tarefa2 = get_absolute_time();
    tarefa2_exibir_oled(media, t);
    fim_tarefa2 = get_absolute_time();
}

/*******************************/
void tarefa_4() {
    ini_tarefa4 = get_absolute_time();
    tarefa4_matriz_cor_por_tendencia(t);
    fim_tarefa4 = get_absolute_time();
}

/*******************************/
void tarefa_5() {
    while (media < 1) {
        npSetAll(COR_BRANCA);
        npWrite();
        sleep_ms(1000);
        npClear();
        npWrite();
        sleep_ms(1000);
    }
}
