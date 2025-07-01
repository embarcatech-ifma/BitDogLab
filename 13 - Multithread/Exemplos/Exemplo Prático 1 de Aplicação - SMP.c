/*
================================================================================
Exemplo 1 de Aplicação - SMP
(Item 8.5 do E-Book)

ATENÇÃO:

Configuração necessária no FreeRTOSConfig.h

Verifique se estes defines estão ativos:

#define configNUMBER_OF_CORES 2
#define configUSE_CORE_AFFINITY 1
#define configRUN_MULTIPLE_PRIORITIES 1

Defina no FreeRTOSConfig.h , caso não tenha:
#define configUSE_PASSIVE_IDLE_HOOK 0

=======================================================
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t print_mutex;

// Funçao de impressao protegida por mutex
void safe_print(const char *msg) {
  xSemaphoreTake(print_mutex, portMAX_DELAY);
  printf("%s\n", msg);
  xSemaphoreGive(print_mutex);
}

// Funçao de tarefa
void vTaskPrint(void *pvParameters) {
  const char *task_name = (const char *) pvParameters;
  char msg[64];
  while (1) {
    snprintf(msg, sizeof(msg), "Tarefa %s rodando no core %d", task_name, get_core_num());
    safe_print(msg);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

int main() {
  stdio_init_all();
  sleep_ms(2000); // Pequeno delay para inicializar USB Serial
  print_mutex = xSemaphoreCreateMutex();
  TaskHandle_t taskA_handle, taskB_handle;
  
  // Criar as 4 tarefas
  xTaskCreate(vTaskPrint, "Tarefa_A", 256, "A", 1, &taskA_handle);
  xTaskCreate(vTaskPrint, "Tarefa_B", 256, "B", 1, &taskB_handle);
  xTaskCreate(vTaskPrint, "Tarefa_C", 256, "C", 1, NULL);
  xTaskCreate(vTaskPrint, "Tarefa_D", 256, "D", 1, NULL);
  
  // Afinidade de nu cleo (core pinning)
  vTaskCoreAffinitySet(taskA_handle, (1 << 0)); // Core 0
  vTaskCoreAffinitySet(taskB_handle, (1 << 1)); // Core 1
  
  // Iniciar escalonador
  vTaskStartScheduler();
  
  while (1); // Nunca chega aqui
}
