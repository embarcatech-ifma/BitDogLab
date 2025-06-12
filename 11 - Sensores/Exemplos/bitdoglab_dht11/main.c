#include <stdio.h>
#include "pico/stdlib.h"
#include "dht11.h"

#define DHT_PIN 15

int main() {
    stdio_init_all();
    dht11_init(DHT_PIN);

    while (true) {
        dht11_result_t res = dht11_read();
        if (res.success) {
            printf("Temperatura: %d°C, Umidade: %d%%\n", res.temperature, res.humidity);
        } else {
            printf("Erro ao ler o DHT11\n");
        }
        sleep_ms(2000);
    }
}
