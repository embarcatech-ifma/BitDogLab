/*
 * =============================================================================================
 * Exemplo 7 - Criação, Prioridade, Escalonabilidade e Deleção de Tarefas no FreeRTOS com Pico W
 * =============================================================================================
 * 
 * Objetivo Geral:
 * Demonstrar:
 * - Criação de tarefas no FreeRTOS.
 * - Atribuição de prioridades (RM e RR).
 * - Conceito de escalonabilidade.
 * - Exclusão dinâmica de tarefas durante a execução.
 * - Utilização de handles de tarefas para gerenciar execução e deleção.
 * 
 * Conceitos abordados:
 * ---------------------------------------------------------------------------
 * - xTaskCreate(): cria tarefas.
 * - vTaskDelete(): exclui tarefas.
 * - Handles (TaskHandle_t): ponteiro que referencia uma tarefa criada.
 *   --> Permite manipular qualquer tarefa externamente.
 * 
 * Como funciona o Handle:
 * ---------------------------------------------------------------------------
 * - Ao criar uma tarefa, passamos o endereço do handle:
 *      xTaskCreate(taskA, "TaskA", 1024, NULL, 3, &taskAHandle);
 * - O FreeRTOS preenche automaticamente o handle.
 * - Podemos usar o handle depois para deletar a tarefa:
 *      vTaskDelete(taskAHandle);
 * - Se passarmos NULL: vTaskDelete(NULL); → a própria tarefa se autodeleta.
 * 
 * Políticas de Escalonamento:
 * ---------------------------------------------------------------------------
 * 1️. Rate Monotonic (RM): maior prioridade para menor período.
 * 2️. Round Robin (RR): mesmas prioridades, FreeRTOS fatia o tempo.
 * 
 * Escalonabilidade (Liu & Layland):
 * ---------------------------------------------------------------------------
 * U = Σ (C_i / T_i)
 * Limite seguro: U ≤ n(2^(1/n) - 1)
 * 
 * Aplicações práticas reais:
 * ---------------------------------------------------------------------------
 * ✅ Tarefas temporárias de inicialização de periféricos.
 * ✅ Autodiagnóstico de hardware (Self-test).
 * ✅ Modos de operação transitórios (modo seguro, modo teste, etc).
 * ✅ Supervisão de watchdogs e timeout de processos.
 * ✅ Sistemas de baixo consumo que eliminam tarefas ociosas.
 * ✅ Robótica, CLP, IoT embarcado, controle de produção.
 */


#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"

// Handles das tarefas
TaskHandle_t taskAHandle = NULL;
TaskHandle_t taskBHandle = NULL;
TaskHandle_t taskCHandle = NULL;
TaskHandle_t managerTaskHandle = NULL;

// Tarefa A
void taskA(void *parameter) {
    while (1) {
        printf("Executando Tarefa A\n");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Tarefa B
void taskB(void *parameter) {
    while (1) {
        printf("Executando Tarefa B\n");
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// Tarefa C
void taskC(void *parameter) {
    while (1) {
        printf("Executando Tarefa C\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Tarefa Gerenciadora: deleta a Tarefa C após 10 segundos
void managerTask(void *parameter) {
    printf("Gerenciador aguardando...\n");
    vTaskDelay(pdMS_TO_TICKS(10000));  // Aguarda 10s

    if (taskBHandle != NULL) {
        printf("Deletando Tarefa B...\n");
        vTaskDelete(taskBHandle);
        taskBHandle = NULL;
        printf("Tarefa B deletada com sucesso!\n");
    }

    if (taskCHandle != NULL) {
        printf("Deletando Tarefa C...\n");
        vTaskDelete(taskCHandle);
        taskCHandle = NULL;
        printf("Tarefa C deletada com sucesso!\n");
    }

    vTaskDelete(NULL);  // Autodeleta o próprio gerenciador
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Iniciando sistema FreeRTOS com demonstração completa...\n");

    // ===========================
    // Configuração de prioridades
    // ===========================

    // ----------- Modo Rate Monotonic (descomente para testar RM) -----------
    xTaskCreate(taskA, "TaskA", 1024, NULL, 3, &taskAHandle);
    xTaskCreate(taskB, "TaskB", 1024, NULL, 2, &taskBHandle);
    xTaskCreate(taskC, "TaskC", 1024, NULL, 1, &taskCHandle);

    // ----------- Modo Round Robin (descomente para testar RR) -----------
    /*
    xTaskCreate(taskA, "TaskA", 1024, NULL, 1, &taskAHandle);
    xTaskCreate(taskB, "TaskB", 1024, NULL, 1, &taskBHandle);
    xTaskCreate(taskC, "TaskC", 1024, NULL, 1, &taskCHandle);
    */

    // Criação do gerenciador
    xTaskCreate(managerTask, "ManagerTask", 1024, NULL, 4, &managerTaskHandle);

    vTaskStartScheduler();

    while (true) {}
}
