#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

// ⚙️ CONFIGURAÇÃO DA REDE
#define WIFI_SSID "Gomez"
#define WIFI_PASS "32433108"
#define WIFI_AUTH CYW43_AUTH_WPA2_AES_PSK // ou CYW43_AUTH_OPEN para rede sem senha

#define MAX_TENTATIVAS 5
#define TIMEOUT_CONEXAO_MS 15000

int main() {
    stdio_init_all();
    sleep_ms(3000); // Aguarda USB estabilizar

    printf("Iniciando módulo Wi-Fi...\n");

    if (cyw43_arch_init()) {
        printf("❌ Erro ao inicializar o CYW43439.\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    bool conectado = false;
    for (int tentativa = 1; tentativa <= MAX_TENTATIVAS; tentativa++) {
        printf("🔄 Tentativa %d de conexão com a rede: %s\n", tentativa, WIFI_SSID);

        int status = cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID,
            (WIFI_AUTH == CYW43_AUTH_OPEN) ? NULL : WIFI_PASS,
            WIFI_AUTH,
            TIMEOUT_CONEXAO_MS
        );

        if (status == 0) {
            conectado = true;
            break;
        } else {
            printf("⚠️  Falha ao conectar (código de erro: %d)\n", status);
            sleep_ms(2000); // espera antes da próxima tentativa
        }
    }

    if (!conectado) {
        printf("❌ Todas as tentativas falharam. Encerrando...\n");
        return 1;
    }

    printf("✅ Conectado com sucesso à rede Wi-Fi: %s\n", WIFI_SSID);

    uint8_t *ip = (uint8_t*)&cyw43_state.netif[0].ip_addr.addr;
    printf("📡 Endereço IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

    // Loop principal
    while (true) {
        cyw43_arch_poll();
        sleep_ms(1000);
    }

    cyw43_arch_deinit();
    return 0;
}
