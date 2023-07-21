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
#include "drv_spi.h"
#include <stdio.h>
/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

static struct rt_thread thread;
static char thread_stack[1024];
static struct rt_thread thread;
struct rt_spi_device ccm;

#define CS_PIN GET_PIN(B, 12)
#define GINT0  GET_PIN(B, 1)
#define GINT1  GET_PIN(B, 2)
#define POR    GET_PIN(B, 0)

void ccm3310_init(void)
{
    rt_pin_mode(POR, PIN_MODE_OUTPUT);
    rt_pin_mode(GINT0, PIN_MODE_OUTPUT);
    rt_pin_mode(GINT1, PIN_MODE_INPUT);

    rt_err_t err = rt_spi_bus_attach_device_cspin(&ccm, "ccm", "spi2", CS_PIN, RT_NULL);
    if (err != RT_NULL) {
        printf("Fail to attach %s creating spi_device %s failed.\n", "spi2", "ccm");
        return;
    }

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode       = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
    cfg.max_hz     = 10 * 1000 * 1000;
    rt_spi_configure(&ccm, &cfg);
    rt_pin_write(POR, PIN_LOW);
}

void ccm3310_test(void *p)
{

    uint8_t data[20] = {
        0x55, 0x02, 0x10, 0x33,
        0x50, 0x00, 0x00, 0x00,
        0x80, 0x30, 0x00, 0x00,
        0x55, 0x55, 0x55, 0x55,
        0x55, 0x02, 0x33, 0x01};

    // uint8_t data[20] = {0x55, 0x02, 0x33, 0x01, 0x55, 0x55, 0x55, 0x55, 0x00,0x00, 0x30, 0x80, 0x50, 0x00, 0x00, 0x00, 0x53, 0x02, 0x10, 0x33};
    // uint8_t data[20] = {0x33, 0x10, 0x02, 0x53, 0x00, 0x00, 0x00, 0x50, 0x80, 0x30, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x01, 0x33, 0x02, 0x55};
    uint8_t r[100] = {0};
    // uint8_t r[100] = {
    //     0x52, 0x02, 0x10, 0x33,
    //     // 数据长度
    //     0x50, 0x00, 0x00, 0x00,
    //     // 状态字
    //     0x00, 0x90,
    //     0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    //     // 数据区
    //     0x43, 0x43, 0x4d, 0x33,
    //     0x33, 0x31, 0x30, 0x53,
    //     0x2d, 0x54, 0x20, 0x53,
    //     0x50, 0x49, 0x20, 0x41,
    //     0x4c, 0x47, 0x4f, 0x20,
    //     0x43, 0x4f, 0x53, 0x20,
    //     0x56, 0x31, 0x2e, 0x33,
    //     0x30, 0x20, 0x32, 0x30,
    //     0x31, 0x39, 0x2e, 0x31,
    //     0x31, 0x2e, 0x32, 0x36,
    //     0x00, 0x00, 0x00, 0x00,
    //     0x00, 0x00, 0x00, 0x00,
    //     0x43, 0x43, 0x4d, 0x33,
    //     0x33, 0x31, 0x30, 0x53,
    //     0x2d, 0x54, 0x20, 0x51,
    //     0x46, 0x4e, 0x33, 0x32,
    //     0x00, 0x00, 0x00, 0x00,
    //     0x00, 0x00, 0x00, 0x00,
    //     0x00, 0x00, 0x00, 0x00,
    //     0x00, 0x00, 0x00, 0x00,
    //     // 包尾
    //     0x56, 0x02, 0x33, 0x01};
    /*  
        0xf5 0xe3 0xbe 0x4e 
        0x26 0x5e 0x82 0x77 
        0xd4 0x83 0x41 0x2c 
        0x52 0x02 0xfa 0x61 
        0xc1 0xcd 0x7e 0x52 
        0x82 0xfd 0xa0 0x43 
        0x3f 0x4f 0xc2 0xb9 
        0xcd 0x9e 0x0c 0x3b 
        0xd5 0x67 0xbf 0xea 
        0x28 0x38 0xd1 0x57 
        0x53 0xfa 0xd6 0x98 
        0xe9 0x1e 0x11 0xe1 
        0xf0 0xba 0x39 0xe4 
        0x0e 0x5f 0x56 0x3a 
        0x88 0x06 0x07 0x3c 
        0xbd 0x90 0x08 0xa2 
        0x26 0x9f 0x45 0x1e 0x10 0x1b 0x52 0x96 0xfb 0x67 0x2e 0x3a 0x28 0xb8 0xb9 0x81 
        0x5a 0xcd 0xe7 0xba 0x6d 0xa2 0x9d 0xa9 0x5f 0xcf 0x7d 0x9d 0x15 0x81 0x83 0x9f 0xed 0x11 0xf2 0xf9
    */

    struct rt_spi_message message;
    printf("\n=======================\n");
    rt_pin_write(GINT0, PIN_LOW);

    rt_int8_t status = PIN_HIGH;
    while (status == PIN_HIGH) {
        status = rt_pin_read(GINT1);        
        printf("status: 0x%2x\n", status);
    }
    printf("==========begin transmit==========\n");
    rt_spi_transfer(&ccm, data, RT_NULL, sizeof(data));

    while (status == PIN_HIGH) {
        status = rt_pin_read(GINT1);
        printf("status: 0x%2x\n", status);
    }
    printf("==========begin receive ==========\n");

    rt_spi_transfer(&ccm, RT_NULL, r, sizeof(r));

    printf("==========print recvive ==========\n");
    for (size_t i = 0; i < sizeof(r); i++) {
        printf("0x%02x ", r[i]);
        if (i%4==3)
        {
            printf("\n");
        }
        
    }
    printf("\n=======================\n");
}
MSH_CMD_EXPORT(ccm3310_test, test);

struct rt_spi_device *spi_dev_w25q;
#define SPI_CS GET_PIN(C, 13)
void spi_w25q()
{
    rt_uint8_t w25x_read_id = 0x90;
    rt_uint8_t id[5]        = {0};
    spi_dev_w25q            = rt_malloc(sizeof(struct rt_spi_device));
    rt_err_t err            = rt_spi_bus_attach_device_cspin(spi_dev_w25q, "w25q", "spi1", SPI_CS, RT_NULL);
    if (err != RT_NULL) {
        printf("Fail to attach %s creating spi_device %s failed.\n", "spi1", "w25q");
        return;
    }
    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode       = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
    cfg.max_hz     = 10 * 1000 * 1000;
    rt_spi_configure(spi_dev_w25q, &cfg);

    rt_spi_transfer(spi_dev_w25q, &w25x_read_id, RT_NULL, sizeof(w25x_read_id));
    rt_thread_mdelay(100);
    rt_spi_transfer(spi_dev_w25q, RT_NULL, id, 5);
    rt_spi_send_then_recv(spi_dev_w25q, &w25x_read_id, 1, id, 5);
    rt_kprintf("\nuse rt_spi_transfer_message() read w25q ID is:%x%x\n", id[3], id[4]);
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_int8_t status = PIN_HIGH;
    rt_thread_init(&thread, "ccm3310", ccm3310_test, RT_NULL, thread_stack, sizeof(thread_stack), 20, 15);

    // rt_thread_startup(&thread);
    ccm3310_init();
    spi_w25q();
    while (1) {
        // status = rt_pin_read(GINT1);
        // printf("status: %x\n", status);
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
