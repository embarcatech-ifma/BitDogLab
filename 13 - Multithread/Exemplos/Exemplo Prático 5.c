/*
================================================================================
Exemplo prático 5: ISR libera semáforo que aciona LED/buzzer
(Item 6.4 do E-Book)
=======================================================
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#define BUTTON_PIN 5
#define LED_PIN 13

SemaphoreHandle_t count;

void led_task(void *param) {
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  while (true) {
    if (xSemaphoreTake(count, 0) == pdTRUE) {
      gpio_put(LED_PIN, 1);
      vTaskDelay(pdMS_TO_TICKS(1000)); // LED aceso por 1 segundo
    } else {
      gpio_put(LED_PIN, 0);
      vTaskDelay(pdMS_TO_TICKS(100)); // Verificação periódica
    }
  }
}

void button_task(void *param) {
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN); // Importante para evitar estado flutuante
  while (true) {
    if (!gpio_get(BUTTON_PIN)) { // Pressionado (nível baixo)
      xSemaphoreGive(count);
      vTaskDelay(pdMS_TO_TICKS(200)); // Debounce simples
    } else {
      vTaskDelay(pdMS_TO_TICKS(10)); // Pequeno atraso entre leituras
    }
  }
}

int main() {
  stdio_init_all();
  count = xSemaphoreCreateCounting(5, 0);
  xTaskCreate(led_task, "LED", 256, NULL, 1, NULL);
  xTaskCreate(button_task, "BUTTON", 256, NULL, 1, NULL);
  vTaskStartScheduler();
  
  while (true);
  return 0;
}
