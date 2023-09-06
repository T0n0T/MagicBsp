/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_gpio.h"
/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

// #include <fal.h>
// extern int fal_init(void);
// INIT_COMPONENT_EXPORT(fal_init);

int main(void)
{
    /* set LED0 pin mode to output */
    return 0;
}
