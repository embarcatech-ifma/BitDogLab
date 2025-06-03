/**
 * @file conexao.c
 * @brief Núcleo 1 - Cliente Wi-Fi com reconexão automática e envio via FIFO.
 * Envia status da conexão (azul, verde, vermelho), número da tentativa e IP ao núcleo 0.
 */

#include "conexao.h"
#include "wifi_status.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include <stdio.h>
#include <string.h>


uint8_t status_wifi_rgb = 0;

bool wifi_esta_conectado(void) {
    return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP;
}

void enviar_status_para_core0(uint16_t status, uint16_t tentativa) {
    uint32_t pacote = ((tentativa & 0xFFFF) << 16) | (status & 0xFFFF);
    multicore_fifo_push_blocking(pacote);
}

void enviar_ip_para_core0(uint8_t *ip) {
    uint32_t ip_bin = (ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3];
    // Usa tentativa = 0xFFFE para indicar pacote de IP
    uint32_t pacote = (0xFFFE << 16) | 0;
    multicore_fifo_push_blocking(pacote);
    multicore_fifo_push_blocking(ip_bin);
}


void conectar_wifi(void) {
    status_wifi_rgb = 0;
    enviar_status_para_core0(status_wifi_rgb, 0); // inicializando
    

    if (cyw43_arch_init()) {
        status_wifi_rgb = 2;
        enviar_status_para_core0(status_wifi_rgb, 0); // falha init
        return;
    }

    cyw43_arch_enable_sta_mode();

    for (uint16_t tentativa = 1; tentativa <= 5; tentativa++) {
        int result = cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 3000);

        bool conectado = (result == 0) && wifi_esta_conectado();
        status_wifi_rgb = conectado ? 1 : 2;
        enviar_status_para_core0(status_wifi_rgb, tentativa);

        if (conectado) {
            uint8_t *ip = (uint8_t*)&cyw43_state.netif[0].ip_addr.addr;
            enviar_ip_para_core0(ip);
            return;
        }

        sleep_ms(TEMPO_CONEXAO);
    }

    status_wifi_rgb = 2;
    enviar_status_para_core0(status_wifi_rgb, 0);
}

void monitorar_conexao_e_reconectar(void) {
    while (true) {
        sleep_ms(TEMPO_CONEXAO);

        if (!wifi_esta_conectado()) {
            status_wifi_rgb = 2;
            enviar_status_para_core0(status_wifi_rgb, 0);

            cyw43_arch_enable_sta_mode();

            for (uint16_t tentativa = 1; tentativa <= 5; tentativa++) {
                int result = cyw43_arch_wifi_connect_timeout_ms(
                    WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, TEMPO_CONEXAO);

                bool reconectado = (result == 0) && wifi_esta_conectado();
                status_wifi_rgb = reconectado ? 1 : 2;
                enviar_status_para_core0(status_wifi_rgb, tentativa);

                if (reconectado) {
                    uint8_t *ip = (uint8_t*)&cyw43_state.netif[0].ip_addr.addr;
                    enviar_ip_para_core0(ip);

                    break;
                }

                sleep_ms(TEMPO_CONEXAO);
            }

            if (!wifi_esta_conectado()) {
                status_wifi_rgb = 2;
                enviar_status_para_core0(status_wifi_rgb, 0);
            }
        }
    }
}

// Função a ser chamada no núcleo 1
void funcao_wifi_nucleo1(void) {
    conectar_wifi();
    monitorar_conexao_e_reconectar();
}
