#include "pico/stdlib.h"   // Biblioteca padrão para controle de GPIO, temporização e comunicação serial.
#include "hardware/timer.h" // Biblioteca para gerenciamento de temporizadores de hardware.

// Definição dos pinos para o LED e o botão
const uint LED_PIN = 12;    // Define o pino GPIO 12 para o LED.
const uint BUTTON_PIN = 5;  // Define o pino GPIO 5 para o botão.

// Variáveis de controle
bool led_on = false;         // Armazena o estado atual do LED (ligado/desligado).
absolute_time_t turn_off_time;  // Armazena o tempo em que o LED deve ser desligado.
bool led_active = false;     // Indica se o LED está ativo, para evitar reativação durante o tempo de espera.

// Função de callback que será chamada pelo temporizador repetidamente a cada 1 segundo.
bool repeating_timer_callback(struct repeating_timer *t) {
    // Verifica se o LED deve ser desligado:
    // Se o LED está ativo e o tempo atual é maior ou igual ao tempo de desligamento, desliga o LED.
    if (led_active && absolute_time_diff_us(get_absolute_time(), turn_off_time) <= 0) {
        // Define o estado do LED como desligado e atualiza a variável de controle.
        led_on = false;
        gpio_put(LED_PIN, false); // Desliga o LED.
        led_active = false;       // Atualiza a variável para indicar que o LED foi desligado.
    }

    // Retorna true para manter o temporizador repetindo esta função de callback.
    return true;
}

int main() {
    // Inicializa a comunicação padrão, permitindo o uso de printf para depuração.
    // Útil para enviar mensagens pela interface serial (USB ou UART).
    stdio_init_all();    

    // Inicializa e configura o pino do LED (GPIO 12) como saída.
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Inicializa e configura o pino do botão (GPIO 5) como entrada.
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    // Habilita o resistor de pull-up interno no pino do botão.
    // Isso garante que o pino seja lido como alto (3,3 V) quando o botão não está pressionado.
    gpio_pull_up(BUTTON_PIN);

    // Configura um temporizador repetitivo que chama a função de callback a cada 1 segundo (1000 ms).
    struct repeating_timer timer;
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);

    // Loop principal do programa para monitorar o estado do botão e controlar o LED.
    while (true) {
        // Verifica se o botão foi pressionado (nível baixo no pino) e se o LED não está ativo.
        if (gpio_get(BUTTON_PIN) == 0 && !led_active) {
            // Adiciona um pequeno atraso para debounce (aguarda 50 ms para evitar leituras errôneas).
            sleep_ms(50);

            // Verifica novamente o estado do botão após o debounce para confirmar a pressão.
            if (gpio_get(BUTTON_PIN) == 0) {
                // Acende o LED e atualiza o estado e as variáveis de controle.
                led_on = true;
                gpio_put(LED_PIN, true); // Liga o LED.
                led_active = true;

                // Define o tempo para desligar o LED após 2 segundos (2000 ms).
                // `make_timeout_time_ms` calcula um tempo futuro a partir do tempo atual.
                turn_off_time = make_timeout_time_ms(2000);
            }
        }

        // Pequena pausa de 10 ms para reduzir o uso da CPU e evitar execução excessivamente rápida do loop.
        sleep_ms(10);
    }

    // Retorno de 0, que nunca será alcançado devido ao loop infinito.
    // Isso é apenas uma boa prática em programas com um ponto de entrada main().
    return 0;
}
