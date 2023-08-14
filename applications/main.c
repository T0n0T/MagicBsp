/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <board.h>
#include <bsp_lcd_ili9341.h>
#include <lv_example_png.h>
#include <lvgl.h>
#include <rtdevice.h>
#include <rtthread.h>

/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

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
    while (1) {
        // Prepare variable to read out the touch data
        rt_memset(&read_data, 0, sizeof(struct rt_touch_data));
        if (rt_device_read(touch, 0, &read_data, 1) == 1) {
            // Print the touch coordinate and the corresponding information
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

static void event_cb(lv_event_t* e)
{
    LV_LOG_USER("Clicked");

    static uint32_t cnt = 1;
    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "%" LV_PRIu32, cnt);
    cnt++;
}

/**
 * Add click event to a button
 */
void lv_example_event_1(void)
{
    lv_obj_t* btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, "Click me!");
    lv_obj_center(label);
}

static void selectors_create(lv_obj_t* parent);
static void text_input_create(lv_obj_t* parent);
static void msgbox_create(void);

static void msgbox_event_cb(lv_event_t* e);
static void ta_event_cb(lv_event_t* e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_group_t* g;
static lv_obj_t* tv;
static lv_obj_t* t1;
static lv_obj_t* t2;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_keypad_encoder(void)
{
    g = lv_group_create();
    lv_group_set_default(g);

    lv_indev_t* cur_drv = NULL;
    for (;;) {
        cur_drv = lv_indev_get_next(cur_drv);
        if (!cur_drv) {
            break;
        }

        if (cur_drv->driver->type == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(cur_drv, g);
        }

        if (cur_drv->driver->type == LV_INDEV_TYPE_ENCODER) {
            lv_indev_set_group(cur_drv, g);
        }
    }

    tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, LV_DPI_DEF / 3);

    t1 = lv_tabview_add_tab(tv, "Selectors");
    t2 = lv_tabview_add_tab(tv, "Text input");

    selectors_create(t1);
    text_input_create(t2);

    msgbox_create();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void selectors_create(lv_obj_t* parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* obj;

    obj = lv_table_create(parent);
    lv_table_set_cell_value(obj, 0, 0, "00");
    lv_table_set_cell_value(obj, 0, 1, "01");
    lv_table_set_cell_value(obj, 1, 0, "10");
    lv_table_set_cell_value(obj, 1, 1, "11");
    lv_table_set_cell_value(obj, 2, 0, "20");
    lv_table_set_cell_value(obj, 2, 1, "21");
    lv_table_set_cell_value(obj, 3, 0, "30");
    lv_table_set_cell_value(obj, 3, 1, "31");
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_calendar_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_btnmatrix_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_checkbox_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_slider_create(parent);
    lv_slider_set_range(obj, 0, 10);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_switch_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_spinbox_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_dropdown_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    obj = lv_roller_create(parent);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_t* list = lv_list_create(parent);
    lv_obj_update_layout(list);
    if (lv_obj_get_height(list) > lv_obj_get_content_height(parent)) {
        lv_obj_set_height(list, lv_obj_get_content_height(parent));
    }

    lv_list_add_btn(list, LV_SYMBOL_OK, "Apply");
    lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Close");
    lv_list_add_btn(list, LV_SYMBOL_EYE_OPEN, "Show");
    lv_list_add_btn(list, LV_SYMBOL_EYE_CLOSE, "Hide");
    lv_list_add_btn(list, LV_SYMBOL_TRASH, "Delete");
    lv_list_add_btn(list, LV_SYMBOL_COPY, "Copy");
    lv_list_add_btn(list, LV_SYMBOL_PASTE, "Paste");
}

static void text_input_create(lv_obj_t* parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    lv_obj_t* ta1 = lv_textarea_create(parent);
    lv_obj_set_width(ta1, LV_PCT(100));
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_placeholder_text(ta1, "Click with an encoder to show a keyboard");

    lv_obj_t* ta2 = lv_textarea_create(parent);
    lv_obj_set_width(ta2, LV_PCT(100));
    lv_textarea_set_one_line(ta2, true);
    lv_textarea_set_placeholder_text(ta2, "Type something");

    lv_obj_t* kb = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(ta1, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_event_cb(ta2, ta_event_cb, LV_EVENT_ALL, kb);
}

static void msgbox_create(void)
{
    static const char* btns[] = { "Ok", "Cancel", "" };
    lv_obj_t* mbox = lv_msgbox_create(NULL, "Hi", "Welcome to the keyboard and encoder demo", btns, false);
    lv_obj_add_event_cb(mbox, msgbox_event_cb, LV_EVENT_ALL, NULL);
    lv_group_focus_obj(lv_msgbox_get_btns(mbox));
    lv_obj_add_state(lv_msgbox_get_btns(mbox), LV_STATE_FOCUS_KEY);
    lv_group_focus_freeze(g, true);

    lv_obj_align(mbox, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* bg = lv_obj_get_parent(mbox);
    lv_obj_set_style_bg_opa(bg, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(bg, lv_palette_main(LV_PALETTE_GREY), 0);
}

static void msgbox_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* msgbox = lv_event_get_current_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        const char* txt = lv_msgbox_get_active_btn_text(msgbox);
        if (txt) {
            lv_msgbox_close(msgbox);
            lv_group_focus_freeze(g, false);
            lv_group_focus_obj(lv_obj_get_child(t1, 0));
            lv_obj_scroll_to(t1, 0, 0, LV_ANIM_OFF);
        }
    }
}

static void ta_event_cb(lv_event_t* e)
{
    lv_indev_t* indev = lv_indev_get_act();
    if (indev == NULL)
        return;
    lv_indev_type_t indev_type = lv_indev_get_type(indev);

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* ta = lv_event_get_target(e);
    lv_obj_t* kb = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED && indev_type == LV_INDEV_TYPE_ENCODER) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(kb);
        lv_group_set_editing(lv_obj_get_group(kb), kb);
        lv_obj_set_height(tv, LV_VER_RES / 2);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_height(tv, LV_VER_RES);
    }
}

void lv_ex_label(void)
{
    char* github_addr = "https://gitee.com/W23";
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_recolor(label, true);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    lv_obj_set_width(label, 120);
    lv_label_set_text_fmt(label, "#ff0000 Gitee: %s#", github_addr);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t* label2 = lv_label_create(lv_scr_act());
    lv_label_set_recolor(label2, true);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    lv_obj_set_width(label2, 120);
    lv_label_set_text_fmt(label2, "#ff0000 Hello# #0000ff world !123456789#");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, -10);
}

void lv_user_gui_init(void)
{
    // lv_demo_keypad_encoder();
    //    lv_example_event_1();
    lv_example_png_1();
    //    lv_ex_label();
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    rt_thread_t touch_reflect = rt_thread_create("touch_ref", touch_func, RT_NULL, 512, 20, 15);
    //    rt_thread_startup(touch_reflect);

    while (1) {
        lv_task_handler();
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
