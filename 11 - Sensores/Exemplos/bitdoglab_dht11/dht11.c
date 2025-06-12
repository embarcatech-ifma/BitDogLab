#include "dht11.h"
#include "hardware/gpio.h"
#include "pico/time.h"

static uint DHT_PIN;

static inline void set_input() {
    gpio_set_dir(DHT_PIN, GPIO_IN);
}

static inline void set_output() {
    gpio_set_dir(DHT_PIN, GPIO_OUT);
}

static inline void sleep_us_exact(uint32_t us) {
    busy_wait_us(us);
}

void dht11_init(uint gpio) {
    DHT_PIN = gpio;
    gpio_init(DHT_PIN);
    set_output();
    gpio_put(DHT_PIN, 1);
}

dht11_result_t dht11_read() {
    dht11_result_t result = { .temperature = -1, .humidity = -1, .success = false };

    gpio_put(DHT_PIN, 0);
    sleep_ms(20);
    gpio_put(DHT_PIN, 1);
    sleep_us_exact(30);
    set_input();

    sleep_us_exact(40);
    if (gpio_get(DHT_PIN)) return result;
    while (!gpio_get(DHT_PIN));
    while (gpio_get(DHT_PIN));

    uint8_t data[5] = {0};

    for (int i = 0; i < 40; ++i) {
        while (!gpio_get(DHT_PIN));

        sleep_us_exact(30);
        if (gpio_get(DHT_PIN))
            data[i / 8] |= (1 << (7 - (i % 8)));

        while (gpio_get(DHT_PIN));
    }

    uint8_t checksum = data[0] + data[1] + data[2] + data[3];

    if (checksum == data[4]) {
        result.humidity = data[0];
        result.temperature = data[2];
        result.success = true;
    }

    set_output();
    gpio_put(DHT_PIN, 1);

    return result;
}
