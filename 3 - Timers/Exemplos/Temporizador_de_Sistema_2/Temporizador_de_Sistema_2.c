#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico para controle de GPIO, temporização e comunicação serial.
#include "pico/time.h"   // Biblioteca para gerenciamento de temporizadores e alarmes.

const uint LED_PIN = 11;    // Define o pino GPIO 11 para controlar o LED.
const uint BUTTON_PIN = 5;  // Define o pino GPIO 5 para ler o estado do botão.

bool led_on = false;        // Variável global para armazenar o estado do LED (não utilizada neste código).
bool led_active = false;    // Indica se o LED está atualmente aceso (para evitar múltiplas ativações).

// Função de callback para desligar o LED após o tempo programado.
int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    // Desliga o LED configurando o pino LED_PIN para nível baixo.
    gpio_put(LED_PIN, false);

    // Atualiza o estado de 'led_active' para falso, indicando que o LED está desligado.
    led_active = false;

    // Retorna 0 para indicar que o alarme não deve se repetir.
    return 0;
}

int main() {
    // Inicializa a comunicação serial para permitir o uso de printf.
    // Isso é útil para depuração, embora não seja usado neste exemplo.
    stdio_init_all();

    // Configura o pino LED_PIN (11) como saída digital.
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Configura o pino BUTTON_PIN (5) como entrada digital.
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    // Habilita o resistor pull-up interno para o pino do botão.
    // Isso garante que o pino seja lido como alto (3,3 V) quando o botão não está pressionado.
    gpio_pull_up(BUTTON_PIN);

    // Loop principal do programa que verifica continuamente o estado do botão.
    while (true) {
        // Verifica se o botão foi pressionado (nível baixo no pino) e se o LED não está ativo.
        if (gpio_get(BUTTON_PIN) == 0 && !led_active) {
            // Adiciona um pequeno atraso para debounce, evitando leituras errôneas.
            sleep_ms(50);

            // Verifica novamente o estado do botão após o debounce.
            if (gpio_get(BUTTON_PIN) == 0) {
                // Liga o LED configurando o pino LED_PIN para nível alto.
                gpio_put(LED_PIN, true);

                // Define 'led_active' como true para indicar que o LED está aceso.
                led_active = true;

                // Agenda um alarme para desligar o LED após 3 segundos (3000 ms).
                // A função 'turn_off_callback' será chamada após esse tempo.
                add_alarm_in_ms(3000, turn_off_callback, NULL, false);
            }
        }

        // Introduz uma pequena pausa de 10 ms para reduzir o uso da CPU.
        // Isso evita que o loop seja executado muito rapidamente e consuma recursos desnecessários.
        sleep_ms(10);
    }

    // Retorno de 0, que nunca será alcançado devido ao loop infinito.
    // Isso é apenas uma boa prática em programas com um ponto de entrada main().
    return 0;
}
