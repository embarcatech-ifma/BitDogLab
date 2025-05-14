/*
 * Projeto: Conexão Wi-Fi com Raspberry Pi Pico W
 *
 * Objetivo:
 * Conectar o Raspberry Pi Pico W a uma rede Wi-Fi utilizando o módulo CYW43439
 * e exibir o endereço IP atribuído. O programa serve como um teste simples
 * para validar a capacidade de conexão do dispositivo a redes sem fio.
 *
 * Funcionalidades:
 * - Inicialização do módulo Wi-Fi CYW43439.
 * - Conexão a uma rede Wi-Fi via SSID e senha.
 * - Impressão do nome da rede e do IP atribuído.
 * - Manutenção da conexão ativa utilizando polling.
 *
 * Adaptação baseada em exemplo oficial da Raspberry Pi:
 * https://github.com/raspberrypi/pico-examples
 */

#include "pico/cyw43_arch.h"    // Biblioteca para o módulo Wi-Fi CYW43439
#include "pico/stdlib.h"        // Biblioteca padrão para GPIO, delays, stdio etc.
#include "lwip/tcp.h"           // Incluída para possíveis comunicações TCP (não usada aqui)
#include <string.h>
#include <stdio.h>

// Define o nome e a senha da rede Wi-Fi a ser conectada
#define WIFI_SSID "nome da rede"
#define WIFI_PASS "senha"

int main() {
    stdio_init_all();     // Inicializa a comunicação USB serial
    sleep_ms(3000);       // Espera para garantir a conexão da porta serial

    printf("Iniciando conexão Wi-Fi...\n");

    // Inicializa o sistema do módulo CYW43439
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o módulo CYW43439.\n");
        return 1;  // Encerra o programa em caso de erro
    }

    // Ativa o modo "station", que permite se conectar a uma rede existente
    cyw43_arch_enable_sta_mode();

    // Conecta à rede com timeout de 10 segundos
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar à rede Wi-Fi.\n");
        return 1;  // Encerra se não conseguir conectar
    } else {
        printf("Conectado à rede Wi-Fi: %s\n", WIFI_SSID);  // Exibe o SSID
        // Obtém o endereço IP atual do dispositivo
        uint8_t *ip = (uint8_t*)&cyw43_state.netif[0].ip_addr.addr;
        printf("Endereço IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    }

    // Loop infinito para manter a conexão viva
    while (true) {
        cyw43_arch_poll(); // Necessário para manter o funcionamento da pilha de rede
        sleep_ms(1000);    // Aguarda 1 segundo (pode ser ajustado)
    }

    // Encerramento do Wi-Fi (nunca será executado neste código)
    cyw43_arch_deinit();
    return 0;
}
