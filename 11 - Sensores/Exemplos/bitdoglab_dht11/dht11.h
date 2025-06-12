#ifndef DHT11_H
#define DHT11_H

#include "pico/stdlib.h"

typedef struct {
    int temperature;
    int humidity;
    bool success;
} dht11_result_t;

void dht11_init(uint gpio);
dht11_result_t dht11_read();

#endif
