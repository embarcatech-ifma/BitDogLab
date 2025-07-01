/*
=======================================================
TUTORIAL - EXEMPLO 8: Passagem de parâmetro inteiro

Objetivo:
- Demonstrar como passar um número inteiro como parâmetro para uma tarefa no FreeRTOS.

Explicação:
- Um valor inteiro (neste caso, 42) será passado para a tarefa no momento da sua criação.
- Dentro da tarefa, o ponteiro recebido será convertido de volta para inteiro.
- A tarefa imprimirá o valor recebido periodicamente.

Observação:
- A variável passada deve ter escopo estático para continuar válida durante a execução da tarefa.
=======================================================
*/

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void TaskFunction(void *pvParameters) {
    int param = *(int *)pvParameters;
    while (1) {
        printf("Tarefa executada com parâmetro: %d\n", param);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();
    static int taskParam = 42;

    xTaskCreate(TaskFunction, "Task1", 512, &taskParam, 1, NULL);
    vTaskStartScheduler();
    while (1) { }
}
