/**
 * @file main.c
 * @brief Núcleo 0 - Controle principal do sistema embarcado com Raspberry Pi Pico W.
 *
 * Este código é executado no núcleo 0 do RP2040 e desempenha as seguintes funções:
 * - Inicialização da interface OLED para exibição de mensagens ao usuário;
 * - Inicialização do PWM para controle de um LED RGB;
 * - Comunicação com o núcleo 1 por meio de FIFO para receber mensagens relacionadas à conexão Wi-Fi;
 * - Exibição e tratamento das mensagens de status do Wi-Fi;
 * - Inicialização do cliente MQTT após o recebimento do IP válido;
 * - Utilização de fila circular para gerenciar mensagens recebidas.
 *
 * O núcleo 1 é responsável por lidar com a conexão Wi-Fi e envia mensagens de status para este núcleo.
 * O código evita o uso da instrução `continue`, utilizando flags de controle para garantir clareza e controle do fluxo.
 */

#include "fila_circular.h"       // Manipulação de fila circular para armazenar mensagens Wi-Fi
#include "rgb_pwm_control.h"     // Inicialização e controle do LED RGB via PWM
#include "configura_geral.h"     // Configurações globais e estruturas utilizadas no projeto
#include "oled_utils.h"          // Funções auxiliares para escrever no display OLED
#include "ssd1306_i2c.h"         // Driver do display OLED com protocolo I2C
#include "mqtt_lwip.h"           // Cliente MQTT utilizando a pilha lwIP
#include "lwip/ip_addr.h"        // Tipos e estruturas da pilha lwIP
#include "pico/multicore.h"      // Comunicação entre os núcleos do RP2040
#include <stdio.h>               // Entrada e saída padrão
#include "estado_mqtt.h"         // Variáveis de controle do estado do MQTT

// Declarações de funções externas (implementadas em outros arquivos)
extern void funcao_wifi_nucleo1(void);             // Função principal do núcleo 1 (gerencia Wi-Fi)
extern void espera_usb();                          // Espera pela conexão USB com o terminal serial
extern void tratar_ip_binario(uint32_t ip_bin);    // Converte e exibe o IP binário
extern void tratar_mensagem(MensagemWiFi msg);     // Exibe a mensagem Wi-Fi na OLED e trata o status

// Instância da fila circular de mensagens Wi-Fi
FilaCircular fila_wifi;

int main() {
    char mensagem_str[50];  // Buffer para formatação de mensagens

    stdio_init_all();            // Inicializa a entrada/saída padrão (USB Serial)
    setup_init_oled();          // Inicializa a comunicação com o display OLED
    oled_clear(buffer_oled, &area);        // Limpa a tela OLED
    render_on_display(buffer_oled, &area); // Atualiza o display com a tela limpa
    espera_usb();               // Aguarda até que o terminal USB esteja pronto para uso

    // Exibe mensagem de inicialização
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Núcleo 0");
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 16, "Iniciando!");
    render_on_display(buffer_oled, &area);
    sleep_ms(3000);             // Pausa para exibição da mensagem
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);

    // Mensagem indicando que o núcleo 0 está aguardando mensagens do núcleo 1
    printf(">> Núcleo 0 iniciado. Aguardando mensagens do núcleo 1...\n");
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Núcleo 0");
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 16, "Iniciado!");
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 24, "Aguardando mensagens do Núcleo 1...");
    render_on_display(buffer_oled, &area);
    sleep_ms(3000);
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);

    init_rgb_pwm();                // Inicializa o PWM para controle do LED RGB
    fila_inicializar(&fila_wifi); // Inicializa a fila circular de mensagens
    multicore_launch_core1(funcao_wifi_nucleo1); // Inicia o núcleo 1 com a função de Wi-Fi

    while (true) {
        bool mensagem_processada = false;

        // Verifica se há dados disponíveis na FIFO do núcleo 1
        if (multicore_fifo_rvalid()) {
            uint32_t pacote = multicore_fifo_pop_blocking(); // Lê pacote com tentativa e status
            uint16_t tentativa = pacote >> 16;

            if (tentativa == 0xFFFE) {
                // Código especial: pacote contém endereço IP binário
                uint32_t ip_bin = multicore_fifo_pop_blocking(); // Recebe o IP binário
                tratar_ip_binario(ip_bin);                       // Converte e exibe o IP
                mensagem_processada = true;                      // Marca como processado
            }

            if (!mensagem_processada) {
                uint16_t status = pacote & 0xFFFF;

                // Valida o status (espera-se valores 0, 1 ou 2)
                if (status > 2) {
                    snprintf(mensagem_str, sizeof(mensagem_str), "Status inválido: %u (tentativa %u)", status, tentativa);
                    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Status inválido.");
                    render_on_display(buffer_oled, &area);
                    sleep_ms(3000);
                    oled_clear(buffer_oled, &area);
                    render_on_display(buffer_oled, &area);
                    printf("%s\n", mensagem_str);
                    mensagem_processada = true;
                }

                // Se for um status válido, insere na fila para tratamento posterior
                if (!mensagem_processada) {
                    MensagemWiFi msg = {.tentativa = tentativa, .status = status};
                    if (!fila_inserir(&fila_wifi, msg)) {
                        // Caso a fila esteja cheia, descarta a mensagem
                        ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Fila cheia. Descartado.");
                        render_on_display(buffer_oled, &area);
                        sleep_ms(3000);
                        oled_clear(buffer_oled, &area);
                        render_on_display(buffer_oled, &area);
                        printf("Fila cheia. Mensagem descartada.\n");
                    }
                }
            }
        }

        // Remove uma mensagem da fila e a processa
        MensagemWiFi msg_recebida;
        if (fila_remover(&fila_wifi, &msg_recebida)) {
            tratar_mensagem(msg_recebida);  // Mostra status (ex: conectando, falhou, conectado)
        }

        // Após obter IP válido, inicia o cliente MQTT (apenas uma vez)
        if (!mqtt_iniciado && ultimo_ip_bin != 0) {
            printf("[MQTT] Iniciando cliente MQTT...\n");
            iniciar_mqtt_cliente();
            mqtt_iniciado = true;
        }

        sleep_ms(50); // Pequeno atraso para reduzir uso de CPU
    }

    return 0;
}
