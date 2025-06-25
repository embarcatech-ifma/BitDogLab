#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

// 🔧 CONFIGURAÇÃO DE CARGA COMPUTACIONAL
//#define CARGA_LED11 55000   // ~ciclos para tarefa do LED11
//#define CARGA_LED12 75000   // ~ciclos para tarefa do LED12
//#define CARGA_LED13 80000   // ~ciclos para tarefa do LED13

void carga_computacional(int ciclos)
{
    for (volatile int i = 0; i < ciclos; i++) {
        __asm volatile("");  // Garante que o loop não seja otimizado
    }
}

void led_task_11()
{
    const uint LED_PIN = 11;
    bool estado = false;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        absolute_time_t start = get_absolute_time();

        estado = !estado;
        gpio_put(LED_PIN, estado);
        //carga_computacional(CARGA_LED11);

        absolute_time_t end = get_absolute_time();
        int64_t exec_time_us = absolute_time_diff_us(start, end);
        printf("LED11 - Tempo de CPU: %lld us\n", exec_time_us);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void led_task_12()
{
    const uint LED_PIN = 12;
    bool estado = false;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        absolute_time_t start = get_absolute_time();

        estado = !estado;
        gpio_put(LED_PIN, estado);
        //carga_computacional(CARGA_LED12);

        absolute_time_t end = get_absolute_time();
        int64_t exec_time_us = absolute_time_diff_us(start, end);
        printf("LED12 - Tempo de CPU: %lld us\n", exec_time_us);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void led_task_13()
{
    const uint LED_PIN = 13;
    bool estado = false;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        absolute_time_t start = get_absolute_time();

        estado = !estado;
        gpio_put(LED_PIN, estado);
        //carga_computacional(CARGA_LED13);

        absolute_time_t end = get_absolute_time();
        int64_t exec_time_us = absolute_time_diff_us(start, end);
        printf("LED13 - Tempo de CPU: %lld us\n", exec_time_us);

        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    printf("Inicializando tarefas...\n");

    xTaskCreate(led_task_11, "LED11_Task", 256, NULL, 3, NULL);
    xTaskCreate(led_task_12, "LED12_Task", 256, NULL, 2, NULL);
    xTaskCreate(led_task_13, "LED13_Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) {};
}
