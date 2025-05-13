#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"

#define BUFFER_SIZE 1024

uint8_t file_buffer[BUFFER_SIZE];
uint32_t file_index = 0;

int main() {
    stdio_init_all();

    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }

    printf("USB conectado! Envie dados para armazenar.\n");

    while (true) {
        if (tud_cdc_available()) {
            uint8_t buf[64];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));

            for (uint32_t i = 0; i < count; i++) {
                if (file_index < BUFFER_SIZE) {
                    file_buffer[file_index++] = buf[i];  // Armazena no buffer
                }
                printf("Recebido: %c\n", buf[i]); // Exibe caractere recebido
            }

            // Ecoa de volta (opcional)
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }

        tud_task();
    }

    return 0;
}
