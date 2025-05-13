#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"
int main() {
    // Inicializa o USB
    stdio_init_all();
    // Aguarda o USB ser montado
    // Verifica se o host (PC) conectou-se ao dispositivo CDC
    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }
    // Informa via terminal serial que a conexão foi detectada
    printf("USB conectado!\n");

    // Loop principal: ecoa o que receber
    while (true) {
        if (tud_cdc_available()) { // Verifica se há dados disponíveis vindos do host (PC)
            uint8_t buf[64]; // Declara um buffer de 64 bytes
            uint32_t count = tud_cdc_read(buf, sizeof(buf)); // Lê os dados recebidos via USB para esse buffer

            // Para cada byte recebido, imprime no terminal
            for (uint32_t i = 0; i < count; i++) {
                printf("Recebido: %c\n", buf[i]); // Mostra o caractere recebido
            }

            tud_cdc_write(buf, count); // Escreve os mesmos dados de volta ao host, efetivamente fazendo um eco
            tud_cdc_write_flush();
        }
        tud_task(); // Executa tarefas USB
    }

    return 0;
}

