/*
================================================================================
Exemplo prático 1: Tarefa envia dados lidos do ADC, outra atua sobre eles
(Item 4.7 do E-Book)
=======================================================
*/

// --- Inclusão das bibliotecas essenciais do Pico SDK e do FreeRTOS ---
#include "pico/stdlib.h" // Funções de I/O e temporização do Raspberry Pi Pico
#include "hardware/adc.h" // Controle do ADC interno (para leitura analógica)
#include "FreeRTOS.h" // Núcleo do FreeRTOS
#include "task.h" // Funções de criação e controle de tarefas
#include "queue.h" // Funções de criação e manipulação de filas

// --- Mapeamento dos canais ADC utilizados ---
#define ADC_Y 0 // Canal ADC 0 corresponde ao GPIO26 (eixo Y do joystick)
#define ADC_X 1 // Canal ADC 1 corresponde ao GPIO27 (eixo X do joystick)

// --- Pinos de saída digital usados para acionar os LEDs ---
#define LED_X 12 // LED que indica atividade no eixo X
#define LED_Y 13 // LED que indica atividade no eixo Y

// --- Estrutura de dados para armazenar as leituras do joystick ---
typedef struct {
  uint16_t x; // Valor do eixo X (0 a 4095)
  uint16_t y; // Valor do eixo Y (0 a 4095)
} JoystickData_t;

// --- Declaração da fila usada para comunicação entre as tarefas ---
QueueHandle_t fila_joystick;

// --- Tarefa responsável por ler os valores analógicos do joystick ---
void tarefa_joystick(void *param) {
  JoystickData_t leitura; // Variável para armazenar as leituras
  
  // Inicializa o módulo ADC do RP2040
  adc_init();
  
  // Configura os pinos GPIO26 e GPIO27 como entradas analógicas
  adc_gpio_init(26); // VRy → ADC0
  adc_gpio_init(27); // VRx → ADC1
  while (true) {
    
    // Seleciona o canal do eixo X e realiza a leitura
    adc_select_input(ADC_X);
    leitura.x = adc_read();
    
    // Seleciona o canal do eixo Y e realiza a leitura
    adc_select_input(ADC_Y);
    leitura.y = adc_read();
    
    // Envia os dados lidos para a fila (bloqueia até conseguir enviar)
    xQueueSend(fila_joystick, &leitura, portMAX_DELAY);
    
    // Aguarda 100 milissegundos antes de fazer nova leitura
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// --- Tarefa que recebe os dados do joystick e controla os LEDs ---
void tarefa_controle_led(void *param) {
  JoystickData_t dado; // Variável que armazenará os dados recebidos
  while (true) {
    
    // Espera (bloqueia) até receber dados da fila
    if (xQueueReceive(fila_joystick, &dado, portMAX_DELAY) == pdPASS) {
      
      // Liga o LED_X se o valor do eixo X for maior que 3000
      gpio_put(LED_X, (dado.x > 3000));
      
      // Liga o LED_Y se o valor do eixo Y for maior que 3000
      gpio_put(LED_Y, (dado.y > 3000));
    }
  }
}

// --- Função principal do programa ---
int main() {
  
  // Inicializa a comunicação serial padrão (para debug, se necessário)
  stdio_init_all();
  
  // Inicializa os pinos dos LEDs e configura como saída digital
  gpio_init(LED_X);
  gpio_set_dir(LED_X, GPIO_OUT);
  gpio_init(LED_Y);
  gpio_set_dir(LED_Y, GPIO_OUT);
  
  // Cria a fila com capacidade para 10 elementos do tipo JoystickData_t
  fila_joystick = xQueueCreate(10, sizeof(JoystickData_t));
  
  // Verifica se a fila foi criada corretamente
  if (fila_joystick != NULL) {
    
    // Cria a tarefa de leitura do joystick com prioridade 3
    xTaskCreate(tarefa_joystick, "Joystick", 256, NULL, 3, NULL);
    
    // Cria a tarefa de controle dos LEDs com prioridade 2
    xTaskCreate(tarefa_controle_led, "LEDs", 256, NULL, 2, NULL);
    
    // Inicia o escalonador do FreeRTOS
    vTaskStartScheduler();
  } else {
    
    // Caso a fila não seja criada, entra em loop piscando LED_X como sinal de erro
    while (true) {
      gpio_put(LED_X, 1);
      sleep_ms(200);
      gpio_put(LED_X, 0);
      sleep_ms(200);
    }
  }
  return 0;
}