/*
================================================================================
Exemplo prático 6: botão com interrupção libera semáforo binário
(Item 6.5 do E-Book)
=======================================================
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#define BUTTON_PIN 5
#define LED_PIN 13

SemaphoreHandle_t bin_sem;

void gpio_callback(uint gpio, uint32_t events) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (gpio == BUTTON_PIN && events & GPIO_IRQ_EDGE_FALL) {
    xSemaphoreGiveFromISR(bin_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void led_task(void *param) {
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  while (true) {
    if (xSemaphoreTake(bin_sem, portMAX_DELAY) == pdTRUE) {
      gpio_put(LED_PIN, 1);
      vTaskDelay(pdMS_TO_TICKS(500));
      gpio_put(LED_PIN, 0);
    }
  }
}

int main() {
  stdio_init_all();
  bin_sem = xSemaphoreCreateBinary();
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
  xTaskCreate(led_task, "LED Task", 256, NULL, 1, NULL);
  vTaskStartScheduler();
  
  while (true);
}
