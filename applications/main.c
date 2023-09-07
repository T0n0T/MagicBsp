/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include "drv_gpio.h"
#include <board.h>
#include <button.h>

#include <rtdevice.h>
#include <rtthread.h>

/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

#define BUTTON_0 GET_PIN(A, 0) // UP
#define BUTTON_1 GET_PIN(A, 1) // DOWN
#define BUTTON_2 GET_PIN(A, 4) // DOWN

uint8_t read_btn(void)
{
    return (uint8_t)rt_pin_read(BUTTON_0);
}
Button_t btn;

void btn_longpress(void)
{
    rt_kprintf("btn event long press\n");
}

void btn_shortpress(void)
{
    rt_kprintf("btn event short press\n");
}

void btn_double(void)
{
    rt_kprintf("btn event double\n");
}

void btn_continus(void)
{
    rt_kprintf("btn event continus\n");
}

void btn_release(void)
{
    rt_kprintf("btn event release\n");
}

void btn_entry(void* p)
{
    rt_pin_mode(BUTTON_0, PIN_MODE_INPUT);
    Button_Create("btn_test", &btn, read_btn, PIN_HIGH);
    Button_Attach(&btn, BUTTON_DOWM, btn_shortpress);
    Button_Attach(&btn, BUTTON_LONG, btn_longpress);
    Button_Attach(&btn, BUTTON_LONG_FREE, btn_release);
    Button_Attach(&btn, BUTTON_DOUBLE, btn_double);
    while (1) {
        Button_Process();
        rt_thread_mdelay(10);
    }
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_thread_t btn_thread = rt_thread_create("btn_p", btn_entry, 0, 1024, 15, 20);
    rt_thread_startup(btn_thread);
    while (1) {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
