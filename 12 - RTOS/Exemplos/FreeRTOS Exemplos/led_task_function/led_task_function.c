/*
 * ============================
 * Projeto: Piscar LED com FreeRTOS no Raspberry Pi Pico
 * ============================
 *
 * ✅ Objetivo:
 * Demonstrar o uso básico de tarefas com o sistema operacional de tempo real FreeRTOS
 * no microcontrolador Raspberry Pi Pico, controlando um LED através de uma tarefa dedicada.
 *
 * ✅ O que o código faz:
 * - Cria uma única tarefa chamada `led_task_function`, que:
 *   - Configura o pino GPIO 12 como saída digital
 *   - Alterna o estado do LED (ligando e desligando)
 *   - Imprime mensagens no terminal indicando o estado do LED
 *   - Usa `vTaskDelay()` para piscar o LED com intervalos de 500 milissegundos
 *
 * ✅ Recursos utilizados:
 * - FreeRTOS (criação de tarefas, delays, agendador)
 * - GPIO da Raspberry Pi Pico
 * - Comunicação serial para debug (via printf)
 *
 * ✅ O que o código demonstra:
 * - Criação e inicialização de tarefas no FreeRTOS com `xTaskCreate`
 * - Uso de `vTaskDelay(pdMS_TO_TICKS(ms))` para temporização precisa
 * - Controle de GPIOs em ambiente multitarefa
 * - Integração entre FreeRTOS e periféricos do Raspberry Pi Pico
 *
 * ✅ Aplicações:
 * Esse código serve como base introdutória para:
 * - Projetos multitarefa com FreeRTOS
 * - Sistemas embarcados com controle de LEDs, relés ou atuadores
 * - Testes iniciais de temporização e GPIOs
 * - Treinamento de estudantes e iniciantes em RTOS
 */

#include "FreeRTOS.h"
// Inclui as definições principais do FreeRTOS, como tipos e configurações básicas
#include "task.h"
// Inclui funções de gerenciamento de tarefas do FreeRTOS (xTaskCreate, vTaskDelay, etc)
#include <stdio.h>
// Biblioteca padrão de C para entrada e saída (usada aqui para printf)
#include "pico/stdlib.h"
// Biblioteca da Raspberry Pi Pico para controle de GPIO, UART, delays, etc.

TaskHandle_t led_task_handle; // <-- Nome diferente
// Declara um identificador (handle) da tarefa, usado para controle futuro da tarefa (suspender, retomar, deletar)

void led_task_function() // <-- Nome diferente
// Função da tarefa que será executada pelo FreeRTOS
{
// Define o número do GPIO onde o LED está conectado (GPIO 12)
    const uint LED_PIN = 12;
// Inicializa o pino como GPIO
    gpio_init(LED_PIN);
// Configura o pino como saída digital
    gpio_set_dir(LED_PIN, GPIO_OUT);
// Loop infinito da tarefa - executa continuamente
    while (true) {
// Coloca o pino em nível alto - acende o LED
        gpio_put(LED_PIN, 1);
// Imprime que o LED está ligado
        printf("LED ON\n");
// Aguarda 500ms utilizando os ticks do FreeRTOS (libera o processador para outras tarefas)
        vTaskDelay(pdMS_TO_TICKS(500)); // sempre use pdMS_TO_TICKS
// Coloca o pino em nível baixo - apaga o LED
        gpio_put(LED_PIN, 0);
// Imprime que o LED está desligado
        printf("LED OFF\n");
// Aguarda 500ms antes de repetir o ciclo
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Função principal do programa
int main()
// Inicializa entrada/saída padrão, necessária para printf via USB ou UART
{
    stdio_init_all();
// Aguarda 2 segundos antes de iniciar (útil para estabilizar USB)
    sleep_ms(2000);
// Imprime mensagem indicando início do sistema
    printf("Inicializando USB\n");
// Cria uma nova tarefa no FreeRTOS:
//   - Função da tarefa da tarefa a ser executada
    xTaskCreate(
//   - Nome identificador da tarefa
        led_task_function, // função corrigida
// Nome da tarefa para debug
        "LED_Task",

//   - Tamanho da pilha em palavras (não bytes!)
        256,
//   - Parâmetro da tarefa (NULL neste caso)
        NULL,
//   - Prioridade da tarefa (1 = baixa)
        1,
//   - Endereço da variável para armazenar o handle da tarefa
        &led_task_handle // ponteiro armazenado se quiser suspender/resumir depois
    );
// Inicia o escalonador do FreeRTOS. A partir daqui, tarefas começam a ser executadas

    vTaskStartScheduler();
// Loop infinito (por segurança, embora não deva ser alcançado com o escalonador rodando)

    while (1) {};
}