/*
================================================================================
Exemplo 2 de Aplicação - SMP
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
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// --- Definições de pinos ---
#define VRX_PIN 27
#define VRY_PIN 26
#define BUTTON_PIN 5
#define LED_PIN 13

// --- Handles globais ---
QueueHandle_t joystick_queue;
SemaphoreHandle_t usb_mutex;
SemaphoreHandle_t button_semaphore;

// --- Handles de tarefas ---
TaskHandle_t sensor_handle, button_handle, actuator_handle;

// --- Tarefa: Leitura do Joystick ---
void sensor_task(void *param) {
  adc_init();
  adc_gpio_init(VRX_PIN);
  adc_gpio_init(VRY_PIN);
  while (1) {
    uint16_t vrx, vry;
    adc_select_input(1);
    vrx = adc_read();
    adc_select_input(0);
    vry = adc_read();
    uint16_t data[2] = {vrx, vry};
    xQueueSend(joystick_queue, &data, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// --- Tarefa: Leitura do botão ---
void button_task(void *param) {
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN);
  while (1) {
    if (!gpio_get(BUTTON_PIN)) {
      xSemaphoreGive(button_semaphore);
      vTaskDelay(pdMS_TO_TICKS(200)); // debounce
    } else {
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}

// --- Tarefa: LED + USB ---
void actuator_task(void *param) {
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  uint16_t data[2];
  while (1) {
    if (xSemaphoreTake(button_semaphore, 0) == pdTRUE) {
      for (int i = 0; i < 3; i++) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
      }
    }
    if (xQueueReceive(joystick_queue, &data, pdMS_TO_TICKS(10)) == pdTRUE) {
      if (xSemaphoreTake(usb_mutex, pdMS_TO_TICKS(100))) {
        printf("Joystick - VRX: %d, VRY: %d\n", data[0], data[1]);
        xSemaphoreGive(usb_mutex);
      }
      gpio_put(LED_PIN, (data[0] > 3000 || data[1] > 3000) ? 1 : 0);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

int main() {
  stdio_init_all();
  sleep_ms(2000); // Aguarda conexão USB
  joystick_queue = xQueueCreate(5, sizeof(uint16_t[2]));
  usb_mutex = xSemaphoreCreateMutex();
  button_semaphore = xSemaphoreCreateBinary();
  
  // Criação das tarefas
  xTaskCreate(sensor_task, "Sensor", 256, NULL, 1, &sensor_handle);
  xTaskCreate(button_task, "Button", 256, NULL, 1, &button_handle);
  xTaskCreate(actuator_task, "Actuator", 512, NULL, 1, &actuator_handle);
  
  // Afinidade de núcleo (SMP)
  vTaskCoreAffinitySet(sensor_handle, (1 << 0)); // Core 0
  vTaskCoreAffinitySet(button_handle, (1 << 0)); // Core 0
  vTaskCoreAffinitySet(actuator_handle, (1 << 1)); // Core 1
  vTaskStartScheduler();
  
  while (1);
}
