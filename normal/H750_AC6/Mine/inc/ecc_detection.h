#ifndef __ECC_DETECTION_H
#define __ECC_DETECTION_H

#include "bsp.h"

#if UES_RAM_ECC==1
#include "stm32h7xx_hal.h"

void ram_ecc_enable(void);

#endif

#endif

