/*
=======================================================
TUTORIAL - EXEMPLO 10: Monitorando múltiplas tarefas com High Water Mark e handles

Objetivo:
- Demonstrar como monitorar o uso de pilha de várias tarefas, utilizando seus handles.
- Demonstrar o High Water Mark (monitoramento da pilha) que é uma ferramenta de gerenciamento
  de memória em tempo de execução.

Explicação:
- Cada tarefa possui um TaskHandle_t próprio.
- A tarefa de monitoramento usa uxTaskGetStackHighWaterMark(handle) para verificar o uso de pilha de cada tarefa.
- A passagem do handle permite monitorar qualquer tarefa ativa no sistema.

IMPORTANTE:
- Quando usamos NULL como parâmetro, monitoramos a própria tarefa.
- Quando usamos o handle, monitoramos a tarefa identificada.

=======================================================
*/

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

// Handles para as tarefas
TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;

// Tarefa 1
void vTask1(void *pvParameters) {
    while (1) {
        printf("Tarefa 1 em execução\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Tarefa 2
void vTask2(void *pvParameters) {
    while (1) {
        printf("Tarefa 2 em execução\n");
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

// Tarefa de monitoramento
void vTaskMonitor(void *pvParameters) {
    UBaseType_t uxHighWaterMark1, uxHighWaterMark2;

    while (1) {
        uxHighWaterMark1 = uxTaskGetStackHighWaterMark(xTask1Handle);
        uxHighWaterMark2 = uxTaskGetStackHighWaterMark(xTask2Handle);

        printf("High Water Mark - Tarefa 1: %u palavras\n", uxHighWaterMark1);
        printf("High Water Mark - Tarefa 2: %u palavras\n", uxHighWaterMark2);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

int main() {
    stdio_init_all();

    // Criação das tarefas e armazenamento de seus handles
    xTaskCreate(vTask1, "Task1", 512, NULL, 1, &xTask1Handle);
    xTaskCreate(vTask2, "Task2", 512, NULL, 1, &xTask2Handle);
    xTaskCreate(vTaskMonitor, "Monitor", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) { }
}
