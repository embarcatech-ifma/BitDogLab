#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

bool alarme_callback(repeating_timer_t *t) {    
    printf("Alarme disparado!\n");    
    return true;
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    static repeating_timer_t timer;    
    add_repeating_timer_ms(1000, alarme_callback, NULL, &timer);

    while (true) {
        tight_loop_contents();
    }
}
