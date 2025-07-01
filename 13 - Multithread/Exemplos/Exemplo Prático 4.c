/*
================================================================================
Exemplo prático 4: proteção de printf() entre tarefas concorrentes COM Mutex
(Item 5.3 do E-Book)
=======================================================
*/

#include <stdio.h> // Biblioteca padrão
#include "pico/stdlib.h" // Funções da placa Pico
#include "FreeRTOS.h" // Núcleo do FreeRTOS
#include "task.h" // Manipulação de tarefas
#include "semphr.h" // Biblioteca para semáforos e mutexes

SemaphoreHandle_t mutex; // Declaração do mutex

void tarefa1(void *param) {
  while (true) {
    if (xSemaphoreTake(mutex, portMAX_DELAY)) { // Solicita mutex
      for (int i = 0; i < 9; i++) {
        printf("A");
      }
      printf("\n");
      xSemaphoreGive(mutex); // Libera mutex
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void tarefa2(void *param) {
  while (true) {
    if (xSemaphoreTake(mutex, portMAX_DELAY)) {
      for (int i = 0; i < 9; i++) {
        printf("B");
      }
      printf("\n");
      xSemaphoreGive(mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

int main() {
  stdio_init_all();
  mutex = xSemaphoreCreateMutex(); // Criação do mutex
  xTaskCreate(tarefa1, "Tarefa1", 1024, NULL, 1, NULL);
  xTaskCreate(tarefa2, "Tarefa2", 1024, NULL, 1, NULL);
  vTaskStartScheduler();
  
  while (1);
}
