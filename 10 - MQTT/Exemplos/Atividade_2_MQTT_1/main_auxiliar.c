/**
 * @file main_auxiliar.c
 * @brief Funções auxiliares do núcleo 0 no projeto multicore com Raspberry Pi Pico W.
 *
 * Este arquivo complementa a lógica do núcleo 0, com foco na **visualização e interpretação**
 * das mensagens trocadas entre os núcleos via FIFO. Ele oferece suporte ao sistema de
 * exibição no display OLED e ao controle visual do estado da rede por meio de um LED RGB.
 *
 * As funcionalidades principais são:
 * - `espera_usb()`: garante que o terminal USB esteja pronto antes de iniciar a comunicação;
 * - `tratar_mensagem()`: interpreta o status da conexão Wi-Fi (inicializando, conectado ou falha),
 *                        altera o LED RGB e exibe a mensagem no display OLED;
 * - `tratar_ip_binario()`: converte o IP recebido do núcleo 1 (em formato `uint32_t`) para string
 *                          legível e o exibe;
 * - `exibir_status_mqtt()`: exibe no OLED o estado atual do cliente MQTT (iniciado, falhou etc).
 *
 * Este código se comunica com:
 * - `main.c`: que executa o loop principal e recebe as mensagens FIFO do núcleo 1;
 * - `funcao_wifi_nucleo1()`: executada no núcleo 1, que coleta o IP e o status da rede;
 * - Biblioteca `ssd1306_i2c.h`: usada para exibir textos no OLED;
 * - LED RGB controlado por PWM para indicar visualmente o estado de rede.
 */

#include "fila_circular.h"      // Estrutura de mensagens entre os núcleos
#include "rgb_pwm_control.h"    // Controle do LED RGB por PWM
#include "configura_geral.h"    // Parâmetros de configuração geral (ex: PWM_STEP)
#include "oled_utils.h"         // Funções de desenho para o display OLED
#include "ssd1306_i2c.h"        // Interface com o driver do display SSD1306
#include "mqtt_lwip.h"          // Cliente MQTT usando pilha lwIP
#include "lwip/ip_addr.h"       // Tipos para manipulação de IP
#include "pico/multicore.h"     // Comunicação entre núcleos
#include <stdio.h>              // Entrada/saída padrão (ex: printf)
#include "estado_mqtt.h"        // Estado global do cliente MQTT

/**
 * @brief Aguarda até que a conexão USB esteja pronta para comunicação.
 *
 * Esta função evita que o sistema inicie a impressão de mensagens no terminal antes
 * de a porta USB estar visível para o usuário, garantindo que nada seja perdido.
 */
void espera_usb() {
    while (!stdio_usb_connected()) {
        sleep_ms(200); // Espera 200ms entre verificações
    }
    printf("Conexão USB estabelecida!\n");  // Confirma que já pode usar printf
}

/**
 * @brief Interpreta a mensagem Wi-Fi recebida do núcleo 1 e responde visualmente.
 *
 * Esta função trata as mensagens com status 0 (inicializando), 1 (conectado) e 2 (falha).
 * Cada status altera a cor do LED RGB e exibe uma mensagem no OLED.
 * Também imprime no terminal para fins de depuração.
 *
 * @param msg Estrutura contendo tentativa e status da conexão.
 */
void tratar_mensagem(MensagemWiFi msg) {
    const char *descricao = "";

    // Mapeia o status para a mensagem e a cor correspondente no LED RGB
    switch (msg.status) {
        case 0:
            descricao = "INICIALIZANDO";
            set_rgb_pwm(PWM_STEP, 0, 0);  // LED vermelho
            break;
        case 1:
            descricao = "CONECTADO";
            set_rgb_pwm(0, PWM_STEP, 0);  // LED verde
            break;
        case 2:
            descricao = "FALHA";
            set_rgb_pwm(0, 0, PWM_STEP);  // LED azul
            break;
        default:
            descricao = "DESCONHECIDO";
            set_rgb_pwm(PWM_STEP, PWM_STEP, PWM_STEP);  // LED branco
            break;
    }

    // Prepara o texto a ser exibido no display
    char linha_status[32];
    snprintf(linha_status, sizeof(linha_status), "Status do Wi-Fi : %s", descricao);

    // Exibe no OLED
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, linha_status);
    render_on_display(buffer_oled, &area);
    sleep_ms(3000); // Tempo de exibição
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);

    // Mostra no terminal também
    printf("[NÚCLEO 0] Status: %s (%s)\n", descricao, msg.tentativa > 0 ? descricao : "evento");
}

/**
 * @brief Converte o endereço IP de 32 bits (enviado pelo núcleo 1) em string legível.
 *
 * O valor é salvo em `ultimo_ip_bin` e usado para iniciar o cliente MQTT.
 * Também é exibido no display OLED e impresso no terminal.
 *
 * @param ip_bin Endereço IP no formato binário (uint32_t).
 */
void tratar_ip_binario(uint32_t ip_bin) {
    char ip_str[20];
    uint8_t ip[4];

    // Extrai os 4 octetos do IP
    ip[0] = (ip_bin >> 24) & 0xFF;
    ip[1] = (ip_bin >> 16) & 0xFF;
    ip[2] = (ip_bin >> 8) & 0xFF;
    ip[3] = ip_bin & 0xFF;

    // Formata como string "xxx.xxx.xxx.xxx"
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    // Mostra no OLED
    oled_clear(buffer_oled, &area);
    ssd1306_draw_utf8_string(buffer_oled, 0, 0, ip_str);
    render_on_display(buffer_oled, &area);

    // Exibe no terminal
    printf("[NÚCLEO 0] Endereço IP: %s\n", ip_str);

    // Armazena para uso pelo MQTT
    ultimo_ip_bin = ip_bin;
}

/**
 * @brief Exibe no OLED e no terminal o status textual do cliente MQTT.
 *
 * Essa função pode ser chamada por qualquer parte do núcleo 0 após a inicialização
 * ou falha de conexão do cliente MQTT, para informar o usuário.
 *
 * @param texto Mensagem de status ("Conectado", "Desconectado", "Erro", etc.).
 */
void exibir_status_mqtt(const char *texto) {
    // Linha inferior no OLED com prefixo "MQTT: "
    ssd1306_draw_utf8_string(buffer_oled, 0, 16, "MQTT: ");
    ssd1306_draw_utf8_string(buffer_oled, 40, 16, texto);
    render_on_display(buffer_oled, &area);

    // Imprime no terminal
    printf("[MQTT] %s\n", texto);
}
