#include <stdio.h> // Inclui a biblioteca padrão de entrada e saída para uso de funções como printf.
#include "pico/stdlib.h"   // Inclui a biblioteca padrão para funcionalidades básicas como GPIO, temporização e comunicação serial.
#include "hardware/timer.h" // Inclui a biblioteca para gerenciamento de temporizadores de hardware.

// Função de callback que será chamada repetidamente pelo temporizador
// O tipo bool indica que a função deve retornar verdadeiro ou falso para continuar ou parar o temporizador.
bool repeating_timer_callback(struct repeating_timer *t) {
    // Imprime uma mensagem na saída serial indicando que 1 segundo se passou.
    printf("1 segundo passou\n");

    // Retorna true para manter o temporizador repetindo. Se retornar false, o temporizador para.
    return true;
}

int main() {
    // Inicializa a comunicação serial, permitindo o uso de funções como printf.
    // Necessário para enviar mensagens através da interface USB ou UART.
    stdio_init_all();

    // Declaração de uma estrutura de temporizador de repetição.
    // Esta estrutura armazenará informações sobre o temporizador configurado.
    struct repeating_timer timer;

    // Configura o temporizador para chamar a função de callback a cada 1 segundo.
    // Parâmetros:
    // 1000: Intervalo de tempo em milissegundos (1 segundo).
    // repeating_timer_callback: Função de callback que será chamada a cada intervalo.
    // NULL: Dados adicionais passados para a função de callback (não utilizados neste caso).
    // &timer: Ponteiro para a estrutura que armazenará informações do temporizador.
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);

    // Loop infinito que mantém o programa em execução.
    // Aqui você pode adicionar outras tarefas que deseja que o microcontrolador execute.
    while (true) {
        // Pausa de 1 segundo para reduzir o uso da CPU.
        // Embora o temporizador opere independentemente, esta pausa evita que o loop
        // seja executado continuamente sem realizar nada útil.
        sleep_ms(1000);
    }

    // Código nunca chegará aqui devido ao loop infinito, mas é uma boa prática incluir return.
    return 0;
}
