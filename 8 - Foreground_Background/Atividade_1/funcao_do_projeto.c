/*
Função do tipo void para inicializa os pinos
O número do pino (uint pino)
A direção (GPIO_IN ou GPIO_OUT).
Se pull_up: true ou false.
Se pull_down: true ou false. Não funciona no kit.
Uma única função para não repetir código.
*/
#include "funcao_do_projeto.h" // Bibliotecas das includes e funções do projeto.

void inicializar_pino(uint pino, uint direcao, bool pull_up, bool pull_down) {

    if (direcao != GPIO_IN && direcao != GPIO_OUT) {
        return;
    }
    
        gpio_init(pino);            
        gpio_set_dir(pino, direcao); 
    
        if (direcao == GPIO_IN) {
            if (pull_up) {
                gpio_pull_up(pino); 
            } else if (pull_down) {
                gpio_pull_down(pino);
            } else {
                gpio_disable_pulls(pino); 
            }
        } 
    }

    void atuar_no_led(uint pino_, uint delay_)
    {
        gpio_put(pino_, !gpio_get(pino_));
        sleep_ms(delay_);
    }