#include "drv_gpio.h"
#include "drv_spi.h"
#include <stdio.h>

#define CS_PIN GET_PIN(B, 12)

#define POR    GET_PIN(C, 7)
#define GINT0  GET_PIN(C, 8)
#define GINT1  GET_PIN(C, 9)

void ccm3310_init(void);
void ccm3310_thread_start(void);