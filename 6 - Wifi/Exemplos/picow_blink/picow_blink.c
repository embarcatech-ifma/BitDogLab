/*
 * Hello World CYW43439: Piscar o LED do Módulo Wi-Fi no Raspberry Pi Pico W
 *
 * Objetivo:
 * Este código demonstra a inicialização do módulo Wi-Fi CYW43439 presente no Raspberry Pi Pico W
 * e utiliza o LED interno controlado por esse módulo para piscar em intervalos regulares.
 * A aplicação serve como um teste básico ("Hello World") para verificar se o módulo Wi-Fi está 
 * funcionando corretamente.
 *
 * Funcionalidades:
 * - Inicializa o módulo wireless CYW43439.
 * - Verifica se a inicialização ocorreu com sucesso.
 * - Pisca continuamente o LED embutido no módulo Wi-Fi.
 *
 * Adaptação de exemplo oficial disponível no repositório do Raspberry Pi Pico W:
 * https://github.com/raspberrypi/pico-examples
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Biblioteca específica para controle do módulo Wi-Fi CYW43439 (Pico W)

int main() {
    stdio_init_all(); // Inicializa a entrada/saída padrão (para printf, por exemplo)

    // Inicializa o módulo Wi-Fi (CYW43439)
    // Se retornar valor diferente de zero, houve erro na inicialização
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n"); // Mensagem de erro
        return -1; // Encerra o programa com código de erro
    }

    // Loop infinito para piscar o LED controlado pelo módulo Wi-Fi
    while (true) {
        // Liga o LED do módulo Wi-Fi
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250); // Aguarda 250 milissegundos

        // Desliga o LED do módulo Wi-Fi
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250); // Aguarda mais 250 milissegundos
    }
}
