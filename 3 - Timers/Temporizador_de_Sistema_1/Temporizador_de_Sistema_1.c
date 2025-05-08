#include <stdio.h> // Inclui a biblioteca padrão de entrada e saída para uso de funções como printf.
#include "pico/stdlib.h" // Biblioteca padrão para funcionalidades básicas como GPIO, temporização e E/S.
#include "pico/time.h"   // Biblioteca para gerenciamento de tempo, como manipulação de temporizadores e atrasos.

int main() {
    // Inicializa a comunicação serial, permitindo o uso de funções como printf.
    // Isso é essencial para enviar mensagens de depuração via USB ou UART.
    stdio_init_all();

    // Define um intervalo de tempo em milissegundos para o temporizador.
    // Neste caso, o intervalo é de 1000 milissegundos, ou seja, 1 segundo.
    uint32_t interval = 1000; // Intervalo em milissegundos

    // Calcula o próximo tempo absoluto em que a ação deve ocorrer.
    // get_absolute_time() retorna o tempo atual do sistema.
    // delayed_by_us() calcula um tempo futuro adicionando o intervalo em microsegundos ao tempo atual.
    absolute_time_t next_wake_time = delayed_by_us(get_absolute_time(), interval * 1000);

    // Loop infinito que mantém o programa em execução contínua.
    while (true) {
        // Verifica se o tempo atual atingiu ou ultrapassou o tempo definido em next_wake_time.
        // Se sim, a função time_reached() retorna true.
        if (time_reached(next_wake_time)) {
            // Imprime uma mensagem na saída serial indicando que 1 segundo se passou.
            printf("1 segundo passou\n");

            // Calcula o próximo tempo de despertar, adicionando o intervalo de 1 segundo ao tempo atual.
            // Isso garante que a próxima execução aconteça exatamente 1 segundo após a última.
            next_wake_time = delayed_by_us(next_wake_time, interval * 1000);
        }

        // Introduz uma pequena pausa de 1 milissegundo para reduzir o uso da CPU.
        // Isso evita que o loop rode desnecessariamente rápido, economizando recursos.
        sleep_ms(1);
    }
}
