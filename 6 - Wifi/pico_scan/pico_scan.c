/*
 * Hello World CYW43439 - Scanner de Redes Wi-Fi com Raspberry Pi Pico W
 *
 * Objetivo:
 * Este programa realiza varreduras periódicas por redes Wi-Fi disponíveis utilizando o módulo CYW43439
 * do Raspberry Pi Pico W. A cada 10 segundos, ele escaneia as redes próximas e imprime na tela
 * informações como SSID, força do sinal (RSSI), canal, MAC address e tipo de segurança.
 *
 * Funcionalidades:
 * - Inicialização do módulo Wi-Fi (CYW43439).
 * - Ativação do modo estação (station mode).
 * - Escaneamento de redes Wi-Fi próximas.
 * - Impressão formatada das informações de cada rede detectada.
 * - Loop contínuo com verificação de progresso da varredura.
 *
 * Adaptação de exemplo oficial disponível em:
 * https://github.com/raspberrypi/pico-examples
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Biblioteca para o módulo Wi-Fi CYW43439

// Função de callback chamada para cada resultado do escaneamento
static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        // Imprime SSID, intensidade do sinal, canal, MAC e tipo de segurança
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}

#include "hardware/vreg.h"
#include "hardware/clocks.h"

int main() {
    stdio_init_all(); // Inicializa comunicação padrão (printf)

    // Inicializa o módulo Wi-Fi
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    // Ativa o modo estação (STA), necessário para escanear redes
    cyw43_arch_enable_sta_mode();

    // Variáveis de controle do tempo e do status do escaneamento
    absolute_time_t scan_time = nil_time;
    bool scan_in_progress = false;

    while(true) {
        // Verifica se já é hora de escanear novamente
        if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
            if (!scan_in_progress) {
                cyw43_wifi_scan_options_t scan_options = {0}; // Parâmetros padrão

                // Inicia escaneamento Wi-Fi
                int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
                if (err == 0) {
                    printf("\nPerforming wifi scan\n");
                    scan_in_progress = true;
                } else {
                    printf("Failed to start scan: %d\n", err);
                    scan_time = make_timeout_time_ms(10000); // Aguarda 10s antes de tentar novamente
                }
            } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
                // Escaneamento finalizado; agenda o próximo em 10 segundos
                scan_time = make_timeout_time_ms(10000);
                scan_in_progress = false; 
            }
        }

#if PICO_CYW43_ARCH_POLL
        // Modo com polling ativo (útil em sistemas sem interrupção automática do Wi-Fi)
        cyw43_arch_poll(); // Verifica se há trabalho pendente no driver Wi-Fi
        cyw43_arch_wait_for_work_until(scan_time); // Aguarda até próximo evento ou tempo definido
#else
        // Modo com suporte a interrupções (funciona em background)
        sleep_ms(1000); // Espera 1 segundo (pode ser substituído por outra tarefa)
#endif
    }

    // Encerra uso do módulo Wi-Fi (não será executado pois há loop infinito)
    cyw43_arch_deinit();
    return 0;
}
