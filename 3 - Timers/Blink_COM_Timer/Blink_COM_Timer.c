#include <stdio.h> // Biblioteca padrão de entrada e saída para uso de funções como printf.
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico para controle de GPIO, temporização e comunicação serial.
#include "hardware/timer.h" // Biblioteca para controle e configuração de temporizadores de hardware.

// Definições de pinos
#define LED_VERDE 11
#define LED_AZUL 12

// Função Callback para desligar o LED VERDE que será chamada a cada intervalo definido pelo alarme.
int64_t turn_off_callback_verde(alarm_id_t id, void *user_data) {
    gpio_put(LED_VERDE, 0); // Desliga o LED.
    return 0;
}

// Função Callback para ligar o LED VERDE que será chamada a cada intervalo definido pelo temporizador.
bool repeating_timer_callback(struct repeating_timer *t) {
    gpio_put(LED_VERDE, 1); // Liga o LED.
    // Adiciona um alarme de 3 segundos para chamar a função de Callback que desligará o LED VERDE.
    add_alarm_in_ms(3000, turn_off_callback_verde, NULL, false);
    return 1; // Retorna true para manter o temporizador repetindo esta função.
    // Se retornasse false, o temporizador pararia de chamar a função de callback.
}

int main()
{
    stdio_init_all();

    gpio_init(LED_VERDE); // Inicializa o pino GPIO 11, preparando-o para ser usado como saída digital.
    gpio_set_dir(LED_VERDE, GPIO_OUT); // Configura a direção do pino 11 como saída.
    gpio_put(LED_VERDE, 0); // Garante que o LED VERDE inicia apagado.

    gpio_init(LED_AZUL); // Inicializa o pino GPIO 12, preparando-o para ser usado como saída digital.
    gpio_set_dir(LED_AZUL, GPIO_OUT); // Configura a direção do pino 12 como saída.
    gpio_put(LED_AZUL, 0); // Garante que o LED AZUL inicia apagado.

    // Declara uma estrutura para armazenar informações sobre o temporizador repetitivo.
    struct repeating_timer timer;

    // Configura um temporizador repetitivo que chama a função 'repeating_timer_callback' a cada 5 segundos (5000 ms).
    // Parâmetros:
    // 5000: Intervalo de tempo em milissegundos (5 segundos).
    // repeating_timer_callback: Função de callback que será chamada a cada intervalo.
    // NULL: Dados adicionais que podem ser passados para a função de callback (não utilizado aqui).
    // &timer: Ponteiro para a estrutura que armazenará informações sobre o temporizador.
    add_repeating_timer_ms(5000, repeating_timer_callback, NULL, &timer);

    while (true) {
        
        gpio_put(LED_AZUL, 1); // Acende o LED AZUL.
        sleep_ms(500); // Mantem o LED aceso por 500 ms
        gpio_put(LED_AZUL, 0); // Apaga o LED AZUL.
        sleep_ms(500); // Mantem o LED apagado por 500 ms.

        // O loop infinito é usado para realizar outras tarefas, como por exemplo, piscar o LED AZUL.
        // Como o temporizador e a função de callback estão gerenciando o LED VERDE, nenhuma instrução
        // acerca do LED VERDE é necessária aqui no Loop infinito.
    }
}

