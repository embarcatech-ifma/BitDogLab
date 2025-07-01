/*
 * ============================
 * Projeto: Controle de LED com Medição de Tempo de CPU no FreeRTOS - Raspberry Pi Pico
 * ============================
 *
 * ✅ Objetivo:
 * Demonstrar a criação de tarefas em FreeRTOS que controlam LEDs em diferentes pinos GPIO
 * e medem o tempo de CPU utilizado por cada tarefa, sem realizar carga computacional artificial.
 *
 * ✅ O que o código faz:
 * - Cria três tarefas diferentes (`led_task_11`, `led_task_12`, `led_task_13`)
 * - Cada tarefa:
 *     - Inicializa um pino GPIO como saída
 *     - Alterna seu estado (liga/desliga o LED)
 *     - Mede o tempo de CPU consumido em cada iteração
 *     - Exibe esse tempo via terminal usando `printf`
 *
 * ✅ Recursos utilizados:
 * - FreeRTOS: criação e execução de tarefas
 * - Raspberry Pi Pico: pinos GPIO e temporizador interno
 * - Biblioteca SDK Pico
 *
 * ✅ O que o código demonstra:
 * - Uso de multitarefa com FreeRTOS
 * - Controle de LEDs em diferentes tarefas
 * - Uso da API `get_absolute_time()` para medir tempo de execução
 * - Integração entre FreeRTOS e periféricos do Pico
 *
 * ✅ Aplicações:
 * - Treinamento e ensino sobre sistemas operacionais de tempo real
 * - Análise de desempenho e tempo de CPU em tarefas concorrentes
 * - Projetos com controle de LEDs, sinalização ou indicadores
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

// 🔧 CONFIGURAÇÃO DE CARGA COMPUTACIONAL
// As diretivas abaixo serviriam para simular carga, mas estão comentadas
//#define CARGA_LED11 55000   // ~ciclos para tarefa do LED11
//#define CARGA_LED12 75000   // ~ciclos para tarefa do LED12
//#define CARGA_LED13 80000   // ~ciclos para tarefa do LED13

// Função auxiliar para simular carga computacional (não utilizada atualmente)
void carga_computacional(int ciclos)
{
    // Loop simples para simular uso de CPU
    for (volatile int i = 0; i < ciclos; i++) {
        __asm volatile("");  // Evita que o compilador otimize o loop
    }
}

// Função da tarefa responsável por controlar o LED no GPIO 11
void led_task_11()
{
    const uint LED_PIN = 11;  // Pino GPIO utilizado
    bool estado = false;      // Estado atual do LED

    gpio_init(LED_PIN);                 // Inicializa o pino GPIO
    gpio_set_dir(LED_PIN, GPIO_OUT);   // Configura como saída

    while (true) {
        absolute_time_t start = get_absolute_time();  // Marca o tempo de início

        estado = !estado;              // Alterna o estado do LED
        gpio_put(LED_PIN, estado);     // Aplica o estado ao pino

        // carga_computacional(CARGA_LED11);  // Simula uso de CPU (comentado)

        absolute_time_t end = get_absolute_time();  // Marca o tempo de fim
        int64_t exec_time_us = absolute_time_diff_us(start, end);  // Diferença em microssegundos

        // Exibe o tempo de CPU utilizado nesta iteração
        printf("LED11 - Tempo de CPU: %lld us\n", exec_time_us);
        vTaskDelay(pdMS_TO_TICKS(500));  // Aguarda 500 ms
    }
}

// Função da tarefa para o LED no GPIO 12
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

        // carga_computacional(CARGA_LED12);

        absolute_time_t end = get_absolute_time();
        int64_t exec_time_us = absolute_time_diff_us(start, end);

        printf("LED12 - Tempo de CPU: %lld us\n", exec_time_us);
        vTaskDelay(pdMS_TO_TICKS(700));  // Aguarda 700 ms
    }
}

// Função da tarefa para o LED no GPIO 13
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

        // carga_computacional(CARGA_LED13);

        absolute_time_t end = get_absolute_time();
        int64_t exec_time_us = absolute_time_diff_us(start, end);

        printf("LED13 - Tempo de CPU: %lld us\n", exec_time_us);
        vTaskDelay(pdMS_TO_TICKS(900));  // Aguarda 900 ms
    }
}

// Função principal: ponto de entrada do programa
int main()
{
    stdio_init_all();  // Inicializa comunicação serial para debug

    // Criação das três tarefas, com prioridade 1
    xTaskCreate(led_task_11, "LED11", 256, NULL, 1, NULL);
    xTaskCreate(led_task_12, "LED12", 256, NULL, 1, NULL);
    xTaskCreate(led_task_13, "LED13", 256, NULL, 1, NULL);

    // Inicia o escalonador do FreeRTOS
    vTaskStartScheduler();

    // Nunca deve chegar aqui
    while (true) { }
}
