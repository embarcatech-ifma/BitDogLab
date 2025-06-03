/**
 * @file setup_wifi.h
 * @brief Declaração da função de configuração inicial da conexão Wi-Fi no Raspberry Pi Pico W.
 *
 * Este cabeçalho declara a função `setup()`, responsável por configurar os parâmetros e inicializar
 * os recursos necessários para estabelecer a conexão Wi-Fi no ambiente embarcado.
 *
 * A função `setup()` normalmente realiza as seguintes ações:
 * - Inicialização do módulo de rede (ex: CYW43 no Pico W)
 * - Definição do modo cliente (station mode)
 * - Tentativa de conexão a uma rede Wi-Fi previamente configurada
 * - Configurações iniciais adicionais necessárias antes de iniciar tarefas de rede
 *
 * Este módulo é ideal para ser utilizado na etapa de inicialização do `main()` de projetos IoT,
 * permitindo encapsular e organizar a lógica de conectividade de forma modular.
 */

#ifndef SETUP_OLED_H
#define SETUP_OLED_H

void setup_init_oled(void);

#endif
