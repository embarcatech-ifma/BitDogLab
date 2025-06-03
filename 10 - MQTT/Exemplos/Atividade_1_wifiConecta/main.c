/**
 * @file main.c
 * @brief Núcleo 0 - Recebe mensagens FIFO do núcleo 1: status de conexão e endereço IP.
 * Atualiza o LED RGB, imprime informações e evita leitura incorreta de pacotes.
 */

#include "fila_circular.h"
#include "rgb_pwm_control.h"
#include "configura_geral.h"
#include "oled_utils.h"
#include "ssd1306_i2c.h"




extern void funcao_wifi_nucleo1(void);
void espera_usb();

// Buffer gráfico para o display OLED (com tamanho definido por largura x altura / 8 + controle)
uint8_t buffer_oled[ssd1306_buffer_length];

// Estrutura que define a área de renderização ativa no display
struct render_area area;

FilaCircular fila_wifi;

/**
 * @brief Exibe e interpreta status da conexão.
 */
void tratar_mensagem(MensagemWiFi msg) {
    const char *descricao = "";
    char descricao_str[20];

    switch (msg.status) {
        case 0: descricao = "INICIALIZANDO"; set_rgb_pwm(PWM_STEP, 0, 0); break;
        case 1: descricao = "CONECTADO";     set_rgb_pwm(0, PWM_STEP, 0); break;
        case 2: descricao = "FALHA";      set_rgb_pwm(0, 0, PWM_STEP); break;
        default: descricao = "DESCONHECIDO";         set_rgb_pwm(PWM_STEP, PWM_STEP, PWM_STEP); break;
    }

    if (msg.tentativa == 0) {
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "NÚCLEO 0");
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 8, descricao);
        render_on_display(buffer_oled, &area);
        sleep_ms(5000);
        oled_clear(buffer_oled, &area);
        render_on_display(buffer_oled, &area);
        printf("[NÚCLEO 0] Status: %s (evento)\n", descricao);

    } else {
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "NÚCLEO 0");
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 8, descricao);
        snprintf(descricao_str, sizeof(descricao_str), "TENTATIVA %d", msg.tentativa);
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 8, descricao_str);
        render_on_display(buffer_oled, &area);
        sleep_ms(5000);
        oled_clear(buffer_oled, &area);
        render_on_display(buffer_oled, &area);
        printf("[NÚCLEO 0] Tentativa %u - Status: %s\n", msg.tentativa, descricao);
    }
}

/**
 * @brief Converte e exibe IP recebido como uint32_t.
 */
void tratar_ip_binario(uint32_t ip_bin) {
    char ip_str[20];
    uint8_t ip[4];
    ip[0] = (ip_bin >> 24) & 0xFF;
    ip[1] = (ip_bin >> 16) & 0xFF;
    ip[2] = (ip_bin >> 8) & 0xFF;
    ip[3] = ip_bin & 0xFF;

    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "NÚCLEO 0");
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    ssd1306_draw_utf8_string(buffer_oled, 0, 8, ip_str);
    render_on_display(buffer_oled, &area);
    sleep_ms(5000);
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);
    ssd1306_draw_utf8_string(buffer_oled, 0, 0, ip_str);
    render_on_display(buffer_oled, &area);
    printf("[NÚCLEO 0] Endereço IP: %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
}

int main() {
    char mensagem_str[50];
    stdio_init_all();
    setup_init_oled();
    // Limpa novamente o conteúdo
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);
    //espera_usb();
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Núcleo 0 iniciado. Aguardando mensagens do núcleo 1...");
    render_on_display(buffer_oled, &area);
    sleep_ms(5000);
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);
    printf(">> Núcleo 0 iniciado. Aguardando mensagens do núcleo 1...\n");

    init_rgb_pwm();
    fila_inicializar(&fila_wifi);

    multicore_launch_core1(funcao_wifi_nucleo1);

    while (true) {
        if (multicore_fifo_rvalid()) {
            uint32_t pacote = multicore_fifo_pop_blocking();
            uint16_t tentativa = pacote >> 16;

            // Detecta pacote de IP especial
            if (tentativa == 0xFFFE) {
                uint32_t ip_bin = multicore_fifo_pop_blocking();
                tratar_ip_binario(ip_bin);
                continue;  // evita interpretação como status
            }

            // Trata status normal
            uint16_t status = pacote & 0xFFFF;

            // Validação opcional
            if (status > 2) {
                snprintf(mensagem_str, sizeof(mensagem_str), "Status inválido recebido: %u (tentativa %u)", status, tentativa);
                ssd1306_draw_utf8_multiline(buffer_oled, 0, 16, "Fila Cheia. Mensagem descartada");
                render_on_display(buffer_oled, &area);
                sleep_ms(5000);
                oled_clear(buffer_oled, &area);
                render_on_display(buffer_oled, &area);
                printf("Status inválido recebido: %u (tentativa %u)\n", status, tentativa);
                continue;
            }

            MensagemWiFi msg = {.tentativa = tentativa, .status = status};
            if (!fila_inserir(&fila_wifi, msg)) {
                ssd1306_draw_utf8_multiline(buffer_oled, 0, 16, "Fila Cheia. Mensagem descartada");
                render_on_display(buffer_oled, &area);
                sleep_ms(5000);
                oled_clear(buffer_oled, &area);
                render_on_display(buffer_oled, &area);
                printf("Fila cheia. Mensagem descartada.\n");
            }
        }

        MensagemWiFi msg_recebida;
        if (fila_remover(&fila_wifi, &msg_recebida)) {
            tratar_mensagem(msg_recebida);
        }

        sleep_ms(50);
    }

    return 0;
}

void espera_usb()
{
    while (!stdio_usb_connected()) { sleep_ms(200); }
    printf("Conexão USB estabelecida!\n");
}