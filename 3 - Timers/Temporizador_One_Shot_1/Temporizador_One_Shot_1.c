#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

int64_t my_alarm_callback(alarm_id_t id, void *user_data) {
    // Aqui você coloca o que quer fazer quando o alarme disparar
    printf("Alarme disparou!\n");
    return false; // false para não repetir
}

int main() {
    stdio_init_all();

    // Agendar o alarme para disparar em 10000 ms (10 segundos)
    add_alarm_in_ms(10000, my_alarm_callback, NULL, true);

    while (true) {
        tight_loop_contents(); // Mantém o programa rodando
    }
}
