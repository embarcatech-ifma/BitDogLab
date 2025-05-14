/*
 * Projeto: Conexão Wi-Fi com Controle de LED via HTTP - Raspberry Pi Pico W
 *
 * Objetivo:
 * Conectar o Raspberry Pi Pico W a uma rede Wi-Fi utilizando o módulo CYW43439,
 * iniciar um servidor HTTP na porta 80 e permitir o controle de um LED por meio de
 * comandos recebidos via navegador web. O sistema também exibe o nome da rede conectada
 * e o endereço IP atribuído dinamicamente.
 *
 * Funcionalidades:
 * - Inicialização e conexão à rede Wi-Fi.
 * - Impressão do SSID e endereço IP no terminal.
 * - Servidor HTTP embarcado com controle de LED via browser.
 * - Comandos: /led/on (liga) e /led/off (desliga).
 * - Loop contínuo para manter a conexão ativa com polling.
 *
 * Baseado em exemplo oficial da Raspberry Pi:
 * https://github.com/raspberrypi/pico-examples
 */

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define LED_PIN 12               // Pino GPIO utilizado para o LED
#define WIFI_SSID "sampaio"      // Nome da rede Wi-Fi
#define WIFI_PASS "diegosampaio" // Senha da rede Wi-Fi

// Resposta HTML enviada ao navegador após requisição
#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
                      "<!DOCTYPE html><html><body>" \
                      "<h1>Controle do LED</h1>" \
                      "<p><a href=\"/led/on\">Ligar LED</a></p>" \
                      "<p><a href=\"/led/off\">Desligar LED</a></p>" \
                      "</body></html>\r\n"

// Callback para lidar com requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Extrai e interpreta a requisição HTTP
    char *request = (char *)p->payload;

    if (strstr(request, "GET /led/on")) {
        gpio_put(LED_PIN, 1);  // Liga o LED
    } else if (strstr(request, "GET /led/off")) {
        gpio_put(LED_PIN, 0);  // Desliga o LED
    }

    // Envia a resposta HTML para o cliente
    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);

    // Libera o buffer utilizado
    pbuf_free(p);

    return ERR_OK;
}

// Callback de conexão: define o callback HTTP para conexões novas
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Inicializa o servidor HTTP na porta 80
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();  // Inicializa comunicação USB
    sleep_ms(10000);   // Aguarda a conexão da porta serial

    printf("Iniciando servidor HTTP\n");

    // Inicializa o módulo Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando à rede Wi-Fi...\n");

    // Conecta à rede definida
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar à rede Wi-Fi\n");
        return 1;
    } else {
        // Conexão bem-sucedida
        printf("Conectado à rede Wi-Fi: %s\n", WIFI_SSID);  // Imprime o SSID

        // Lê e imprime o endereço IP atribuído
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP atribuído: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado com sucesso!\n");
    printf("Para ligar ou desligar o LED, acesse o endereço IP seguido de /led/on ou /led/off\n");

    // Inicializa o LED como saída
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Inicia o servidor web
    start_http_server();

    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Mantém a pilha de rede ativa
        sleep_ms(100);      // Pequeno delay para não sobrecarregar a CPU
    }

    // (Este trecho nunca será alcançado, mas está aqui por completude)
    cyw43_arch_deinit();
    return 0;
}
