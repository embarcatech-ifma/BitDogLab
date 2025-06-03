/**
 * @file main_auxiliar.c
 * @brief Funções auxiliares do núcleo 0 no projeto multicore com Raspberry Pi Pico W.
 *
 * Este arquivo complementa a lógica do núcleo 0, com foco na visualização e interpretação
 * das mensagens trocadas entre os núcleos via FIFO. Ele oferece suporte ao sistema de
 * exibição no display OLED e ao controle visual do estado da rede por meio de um LED RGB.
 */

#include "fila_circular.h"
#include "rgb_pwm_control.h"
#include "configura_geral.h"
#include "oled_utils.h"
#include "ssd1306_i2c.h"
#include "mqtt_lwip.h"
#include "lwip/ip_addr.h"
#include "pico/multicore.h"
#include <stdio.h>
#include "estado_mqtt.h"

/**
 * @brief Aguarda até que a conexão USB esteja pronta para comunicação.
 */
void espera_usb() {
    while (!stdio_usb_connected()) {
        sleep_ms(200);
    }
    printf("Conexão USB estabelecida!\n");
}

/**
 * @brief Trata mensagens recebidas pela FIFO — status Wi-Fi ou retorno de PING.
 *
 * - Status Wi-Fi (tentativa normal)
 * - Retorno de publicação PING (tentativa == 0x9999)
 */
void tratar_mensagem(MensagemWiFi msg) {
    const char *descricao = "";

    // ======= NOVA LÓGICA: resposta ao PING =======
    if (msg.tentativa == 0x9999) {
        if (msg.status == 0) {
            ssd1306_draw_utf8_multiline(buffer_oled, 0, 32, "ACK do PING OK");
            set_rgb_pwm(0, 65535, 0); // verde
        } else {
            ssd1306_draw_utf8_multiline(buffer_oled, 0, 32, "ACK do PING FALHOU");
            set_rgb_pwm(65535, 0, 0); // vermelho
        }
        render_on_display(buffer_oled, &area);
        return;
    }

    // ======= STATUS Wi-Fi padrão =======
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

    char linha_status[32];
    snprintf(linha_status, sizeof(linha_status), "Status do Wi-Fi : %s", descricao);

    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, linha_status);
    render_on_display(buffer_oled, &area);
    sleep_ms(3000);
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);

    printf("[NÚCLEO 0] Status: %s (%s)\n", descricao, msg.tentativa > 0 ? descricao : "evento");
}

/**
 * @brief Converte IP binário em string, exibe no OLED e salva para uso posterior.
 */
void tratar_ip_binario(uint32_t ip_bin) {
    char ip_str[20];
    uint8_t ip[4];

    ip[0] = (ip_bin >> 24) & 0xFF;
    ip[1] = (ip_bin >> 16) & 0xFF;
    ip[2] = (ip_bin >> 8) & 0xFF;
    ip[3] = ip_bin & 0xFF;

    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    oled_clear(buffer_oled, &area);
    ssd1306_draw_utf8_string(buffer_oled, 0, 0, ip_str);
    render_on_display(buffer_oled, &area);

    printf("[NÚCLEO 0] Endereço IP: %s\n", ip_str);
    ultimo_ip_bin = ip_bin;
}

/**
 * @brief Exibe status textual do MQTT no OLED e terminal.
 */
void exibir_status_mqtt(const char *texto) {
    ssd1306_draw_utf8_string(buffer_oled, 0, 16, "MQTT: ");
    ssd1306_draw_utf8_string(buffer_oled, 40, 16, texto);
    render_on_display(buffer_oled, &area);

    printf("[MQTT] %s\n", texto);
}
