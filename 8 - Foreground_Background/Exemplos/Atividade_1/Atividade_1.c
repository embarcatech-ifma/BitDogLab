#include "funcao_do_projeto.h" // Bibliotecas das includes e funções do projeto.

int main()
{
    stdio_init_all();
    adc_init();

    inicializar_pino(LED_VERMELHO, GPIO_OUT, false, false);
    inicializar_pino(LED_AZUL, GPIO_OUT, false, false);
    inicializar_pino(LED_VERDE, GPIO_OUT, false, false);

    inicializar_pino(BOTAO_A, GPIO_IN, true, false);
    inicializar_pino(BOTAO_B, GPIO_IN, true, false);
    inicializar_pino(BOTAO_C, GPIO_IN, true, false);
    
    while (true) 
    {
        if (!gpio_get(BOTAO_A))         
            atuar_no_led(LED_VERMELHO, DELAY_MS); // Botão A e o LED Vermelho
        if (!gpio_get(BOTAO_B))
            atuar_no_led(LED_AZUL, DELAY_MS); //Botão B e o LED Azul
        if (!gpio_get(BOTAO_C)) 
            atuar_no_led(LED_VERDE, DELAY_MS); // Botão do Joystick e o LED Verde
        //tight_loop_contents();
        //__wfi();
    }
    return 0;//Mesmo que nunca alcance aqui, é o padrão correto.
}
/*
Código com polling, como esta, pode remover o uso da tight_loop_contents(); e __wfi();.
Polling, em sistemas embarcados, significa que o processador verifica repetidamente (em um laço) se um determinado evento aconteceu, 
como o pressionamento de um botão, a chegada de um dado na UART, ou uma mudança em um sensor.
*/