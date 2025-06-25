/*
=======================================================
TUTORIAL - EXEMPLO 9: Passagem de estrutura como parâmetro

Objetivo:
- Demonstrar como passar múltiplos dados (um inteiro e uma string) usando uma estrutura.

Explicação:
- Criamos uma estrutura "TaskParams" com dois campos: id e name.
- Essa estrutura é passada como parâmetro para a tarefa.
- A tarefa recebe o ponteiro, converte e imprime os dados periodicamente.

Observação:
- Como antes, usamos variável estática para manter a validade dos dados durante a execução.
=======================================================
=======================================================
APLICAÇÕES PRÁTICAS (com exemplos de estruturas):

1️⃣ Controle de múltiplos dispositivos com propriedades distintas:
Exemplo: motores com velocidade e sentido de rotação.

typedef struct {
    int motorID;
    int velocidade;
    int sentido; // 0 = horário, 1 = anti-horário
} MotorParams;

2️⃣ Gerenciamento de sensores com calibração individual:
Exemplo: sensores de temperatura com fator de calibração.

typedef struct {
    int sensorID;
    float fatorCalibracao;
    char nomeSensor[20];
} SensorParams;

3️⃣ Tarefas de monitoramento com tempos de amostragem distintos:

typedef struct {
    int idTarefa;
    uint32_t periodoMs;
} PeriodoParams;

4️⃣ Controle de comunicação com diferentes canais (I2C, SPI, UART):

typedef struct {
    int canalID;
    uint8_t endereco;
    uint32_t baudrate;
} ComParams;

5️⃣ Controle de iluminação com diferentes padrões:

typedef struct {
    int idLuz;
    int brilho;
    int modoPiscar; // ex: 0=contínuo, 1=intermitente
} LuzParams;

=======================================================
*/
/*Embora neste exemplo os parâmetros sejam fixos no momento da criação da tarefa, em muitos sistemas embarcados os parâmetros precisam ser ajustados dinamicamente durante a execução.
Por exemplo, imagine um sistema de controle de semáforo inteligente, no qual o tempo de abertura e fechamento do sinal depende do fluxo de veículos detectado por sensores de tráfego.
À medida que o trânsito aumenta, o tempo de abertura do sinal verde pode ser aumentado; à medida que o fluxo diminui, o tempo pode ser reduzido.
Para que esses novos tempos sejam transmitidos à tarefa responsável pelo controle do semáforo durante a execução, é necessário empregar mecanismos de comunicação entre tarefas, como filas (queues), mutexes ou semáforos (no sentido FreeRTOS), que permitem atualizar os parâmetros com segurança, sem risco de conflitos de acesso simultâneo.
Essa capacidade de ajuste dinâmico torna o sistema adaptativo e otimiza o funcionamento, sendo fundamental em sistemas de automação modernos.
Esses recursos serão apresentados em módulos futuros deste curso.*/

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

typedef struct {
    int id;
    char name[20];
} TaskParams;

void TaskFunction(void *pvParameters) {
    TaskParams *params = (TaskParams *)pvParameters;
    while (1) {
        printf("Tarefa ID: %d, Nome: %s\n", params->id, params->name);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    static TaskParams params = {1, "Minha Tarefa"};

    xTaskCreate(TaskFunction, "Task2", 512, &params, 1, NULL);
    vTaskStartScheduler();
    while (1) { }
}
