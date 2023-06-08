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
#include <bsp_lcd_ili9341.h>
#include <lv_example_png.h>
#include <lvgl.h>
#include <board.h>

/* defined the LED0 pin: PB2 */
#define LED0_PIN    GET_PIN(B, 2)

void touch_func(void* p)
{
    rt_device_t touch = rt_device_find("xpt0");
    if (touch == RT_NULL) {
        rt_kprintf("can't find device:%s\n", "xpt0");
        return;
    }
    if (rt_device_open(touch, RT_DEVICE_FLAG_INT_RX) != RT_EOK) {
        rt_kprintf("open device failed!");
        return;
    }

    struct rt_touch_data read_data;
    while (1){
        //Prepare variable to read out the touch data
        rt_memset(&read_data, 0, sizeof(struct rt_touch_data));
        if (rt_device_read(touch, 0, &read_data, 1) == 1)
        {
            //Print the touch coordinate and the corresponding information
            rt_kprintf("%d %d %d %d %d\n",
                        read_data.event,
                        read_data.x_coordinate,
                        read_data.y_coordinate,
                        read_data.timestamp,
                        read_data.width);
            LCD_DrawPoint(read_data.x_coordinate, read_data.y_coordinate, RED);
        }
        rt_thread_mdelay(5);
    }
}

static void event_cb(lv_event_t * e)
{
    LV_LOG_USER("Clicked");

    static uint32_t cnt = 1;
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
    printf("click!\n");
    cnt++;
}

/**
 * Add click event to a button
 */
void lv_example_event_1(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Click me!");
    lv_obj_center(label);
}


void ui_init(void)
{
    lv_example_event_1();
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    LCD_Init();
//    LCD_DisplayDir(5);
//    for (int i = 0; i < 320-1; i++) {
//        for (int j = 0; j < 240-1; j++) {
//            LCD_Fill(i, j, i+1, j+1, BLUE);
//        }
//    }

//    LCD_String(20, 8, "hello", 24, WHITE, BLACK);
//    LCD_GUI();

    rt_thread_t touch_reflect = rt_thread_create("touch_ref", touch_func, RT_NULL, 512, 20, 15);
//    rt_thread_startup(touch_reflect);

    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
