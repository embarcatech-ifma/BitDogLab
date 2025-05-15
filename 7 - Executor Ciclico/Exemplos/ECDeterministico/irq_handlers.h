#ifndef IRQ_HANDLERS_H
#define IRQ_HANDLERS_H

#include <stdbool.h>

extern volatile bool dma_temp_done;
void dma_handler_temp(void);

#endif