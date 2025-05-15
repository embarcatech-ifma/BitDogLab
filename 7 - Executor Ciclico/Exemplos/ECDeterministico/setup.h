#ifndef SETUP_H
#define SETUP_H

#include "hardware/dma.h"

#define DMA_TEMP_CHANNEL 0

extern dma_channel_config cfg_temp;

void setup(void);

#endif
