/**
 * @file main_auxiliar.c
 * @brief Funções auxiliares do núcleo 0 no projeto multicore com Raspberry Pi Pico W.
 *
 * Este arquivo complementa a lógica do núcleo 0, com foco em:
 * - Visualização de mensagens no display OLED.
 * - Interpretação dos dados vindos do núcleo 1 via FIFO.
 * - Controle do LED RGB com base no status da conexão Wi-Fi.
 * - Apresentação do endereço IP recebido.
 * - Atualização do tempo de envio do PING (com feedback visual).
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
#include "estado_mqtt.h"  // Para acesso a intervalo_ping_ms

/**
 * @brief Aguarda até que a conexão USB esteja pronta para comunicação.
 *
 * Enquanto a conexão USB não estiver disponível, bloqueia o avanço
 * do programa com uma espera passiva, verificando a cada 200 ms.
 */
void espera_usb() {
    while (!stdio_usb_connected()) {
        sleep_ms(200);
    }
    printf("Conexão USB estabelecida!\n");
}

/**
 * @brief Trata mensagens recebidas da fila FIFO do núcleo 1.
 *
 * - Se a tentativa for 0x9999, interpreta como resposta ao envio de PING.
 * - Caso contrário, trata como status da conexão Wi-Fi e atualiza o display e LED RGB.
 */
void tratar_mensagem(MensagemWiFi msg) {
    const char *descricao = "";

    // ======= Retorno do PING =======
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

    // ======= Status do Wi-Fi (mensagens regulares) =======
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
 * @brief Converte o endereço IP (binário) para string e exibe no OLED.
 *
 * Além da exibição visual, também salva o valor global `ultimo_ip_bin`
 * para habilitar o cliente MQTT.
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
 * @brief Exibe o status da conexão MQTT no OLED e no terminal.
 *
 * Atualiza a linha 16 do display com a palavra "MQTT: <status>".
 */
void exibir_status_mqtt(const char *texto) {
    ssd1306_draw_utf8_string(buffer_oled, 0, 16, "MQTT: ");
    ssd1306_draw_utf8_string(buffer_oled, 40, 16, texto);
    render_on_display(buffer_oled, &area);

    printf("[MQTT] %s\n", texto);
}

/**
 * @brief Atualiza dinamicamente o tempo de envio do PING.
 *
 * Recebe um novo valor de tempo (em milissegundos) e:
 * - Valida se está entre 1000 e 60000 ms.
 * - Atualiza a variável global `intervalo_ping_ms`.
 * - Exibe o novo valor abaixo da confirmação do ACK.
 */
void set_novo_intervalo_ping(uint32_t novo_intervalo) {
    if (novo_intervalo >= 1000 && novo_intervalo <= 60000) {
        intervalo_ping_ms = novo_intervalo;

        char buffer_msg[32];
        snprintf(buffer_msg, sizeof(buffer_msg), "Intervalo: %u ms", novo_intervalo);

        // Exibe abaixo da linha do ACK (linha 32 → y = 42 px)
        ssd1306_clear_area(buffer_oled, 0, 40, 127, 50);
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 42, buffer_msg);
        render_on_display(buffer_oled, &area);

        printf("[INFO] Intervalo atualizado para %u ms\n", novo_intervalo);
    } else {
        printf("[AVISO] Valor %u fora do intervalo permitido (1000–60000 ms)\n", novo_intervalo);
    }
}


/**
 * @brief Exibe no OLED e no terminal a cor RGB ativada.
 *
 * Recebe o código RGB (0 a 7), interpreta a cor correspondente,
 * exibe no display (linha inferior) e imprime no terminal.
 */
/**
 * @brief Exibe no OLED e no terminal a cor RGB ativada.
 *
 * Recebe o código RGB (0 a 7), interpreta a cor correspondente,
 * exibe no display (linha inferior) e imprime no terminal.
 */
void mostrar_cor_rgb(uint8_t codigo) {
    const char *nome_cor = "RGB: ---";

    switch (codigo) {
        case 0: nome_cor = "RGB: APAGADO";   break;
        case 1: nome_cor = "RGB: AZUL";      break;
        case 2: nome_cor = "RGB: VERDE";     break;
        case 3: nome_cor = "RGB: CIANO";     break;
        case 4: nome_cor = "RGB: VERMELHO";  break;
        case 5: nome_cor = "RGB: MAGENTA";   break;
        case 6: nome_cor = "RGB: AMARELO";   break;
        case 7: nome_cor = "RGB: BRANCO";    break;
        default:
            printf("[NÚCLEO 0] Código RGB inválido: %u\n", codigo);
            return;
    }

    printf("[NÚCLEO 0] Cor exibida no OLED: %s\n", nome_cor);

    // Atualiza a linha inferior do OLED
    ssd1306_clear_area(buffer_oled, 0, 54, 127, 63);  // limpa linha inferior
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 56, nome_cor);
    render_on_display(buffer_oled, &area);
    sleep_ms(3500);
    ssd1306_clear_area(buffer_oled, 0, 54, 127, 63);  // limpa linha inferior
    render_on_display(buffer_oled, &area);
}
