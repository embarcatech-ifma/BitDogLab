/*
================================================================================
Exemplo 11: Suspender e Reiniciar Tarefas no FreeRTOS (Pico W)

Objetivo:
- Demonstrar o uso de vTaskSuspend() e vTaskResume() no FreeRTOS.

Explicação resumida:
- Task1: executa repetidamente uma atividade simulada.
- Task2: suspende e reinicia Task1 a cada 5 segundos.

IMPORTANTE:
- Sempre use o handle correto da tarefa para suspender ou retomar.
- A prioridade de Task2 é superior para garantir que controle a suspensão.

================================================================================
APLICAÇÕES PRÁTICAS:

1. Economia de energia em sensores:
- Ex.: suspender leitura de sensores quando o ambiente está inativo.

2 Gerenciamento de comunicação periódica:
- Ex.: tarefas de WiFi, LoRa ou Bluetooth só são ativadas quando há necessidade de comunicação.

3 Controle de atuadores condicionais:
- Ex.: suspender tarefas que controlam motores ou válvulas até que sejam acionados.

4 Modos operacionais distintos:
- Ex.: suspender tarefas de diagnóstico enquanto o sistema opera normalmente.

5 Priorização dinâmica de carga:
- Ex.: suspender tarefas não-críticas durante picos de processamento.

6. Supervisão adaptativa de tarefas (gestão inteligente do sistema)

Exemplos de monitoramento:
- Estado da bateria (nível baixo de carga)
- Sensores ausentes (não presentes na alive-list ou falha de inicialização)
- Comunicação com rede indisponível
- Temperatura ou tensão fora de faixa segura

A lógica básica:

- Quando o sistema detecta alguma anomalia ou restrição de energia:
    -> Suspende tarefas não essenciais:
       - Leitura de sensores desconectados
       - Processamentos de dados não prioritários
       - Comunicação remota ou upload de dados

- Quando as condições retornam ao normal:
    -> Retoma as tarefas suspensas.

Esse controle adaptativo otimiza o consumo de energia, aumenta a robustez e mantém o sistema 
funcionando de forma segura em ambientes com restrições.

Exemplo prático:
- Se um sensor de temperatura falha ou não é detectado, a tarefa de leitura desse sensor 
  pode ser suspensa automaticamente até que o sensor volte a estar presente.

=======================================================
*/


#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

// Handles das tarefas
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

// Função da Task1
void task1(void *parameter) {
    while (1) {
        printf("Task1: Executando atividade...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Função da Task2
void task2(void *parameter) {
    while (1) {
        printf("Task2: Suspendendo a Task1...\n");
        vTaskSuspend(task1Handle);  // Suspende a Task1

        printf("Task2: Task1 suspensa por 5 segundos.\n");
        vTaskDelay(pdMS_TO_TICKS(5000));

        printf("Task2: Retomando a Task1...\n");
        vTaskResume(task1Handle);  // Retoma a Task1

        printf("Task2: Task1 retomada. Aguardando 5 segundos...\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

int main() {
    stdio_init_all();  // Inicializa USB serial
    sleep_ms(2000);

    printf("Iniciando o Sistema FreeRTOS no Pico W...\n");

    // Criando a Task1
    xTaskCreate(task1, "Task1", 512, NULL, 1, &task1Handle);

    // Criando a Task2
    xTaskCreate(task2, "Task2", 512, NULL, 2, &task2Handle);

    printf("Task1 e Task2 criadas com sucesso!\n");

    vTaskStartScheduler();  // Inicia o escalonador do FreeRTOS

    while (1) { }  // Nunca deve chegar aqui
}
