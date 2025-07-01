/*
================================================================================
Exemplo prático 3: proteção de printf() entre tarefas concorrentes SEM Mutex
(Item 5.3 do E-Book)
=======================================================
*/

#include <stdio.h> // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h" // Inicialização da placa Pico
#include "FreeRTOS.h" // Cabeçalho principal do FreeRTOS
#include "task.h" // Suporte a tarefas do FreeRTOS

// Tarefa que imprime o caractere A nove vezes
void tarefa1(void *param) {
  while (true) {
    for (int i = 0; i < 9; i++) {
      printf("A");
    }
    printf("\n"); // Nova linha após imprimir
    vTaskDelay(pdMS_TO_TICKS(200)); // Espera 200 ms
  }
}

// Tarefa que imprime o caractere B nove vezes
void tarefa2(void *param) {
  while (true) {
    for (int i = 0; i < 9; i++) {
      printf("B");
    }
    printf("\n");
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

int main() {
  stdio_init_all(); // Inicializa USB serial
  xTaskCreate(tarefa1, "Tarefa1", 1024, NULL, 1, NULL);
  xTaskCreate(tarefa2, "Tarefa2", 1024, NULL, 1, NULL);
  vTaskStartScheduler(); // Inicia o escalonador do FreeRTOS
  
  while (1); // Loop infinito
}
