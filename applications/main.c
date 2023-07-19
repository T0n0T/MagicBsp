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

void ccm3310_test(void *p)
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

    printf("print berfore\n");
    for (size_t i = 0; i < sizeof(r); i++) {
        printf("0x%02x ", r[i]);
    }
    printf("\n=======================\n");
    printf("begin test:\n");
    rt_pin_write(POR, PIN_LOW);
    rt_pin_write(GINT0, PIN_LOW);

    rt_int8_t status = 0x01;
    while (status) {
        status = rt_pin_read(GINT1);
        printf("in while\n");
        printf("status: 0x%2x\n", status);
        rt_thread_mdelay(200);
    }

    rt_spi_send_then_recv(&ccm, data, 20, r, sizeof(r));

    printf("print after\n");
    for (size_t i = 0; i < sizeof(r); i++) {
        printf("0x%02x ", r[i]);
    }
    printf("\n");
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

    rt_spi_send_then_recv(spi_dev_w25q, &w25x_read_id, 1, id, 5);
    rt_kprintf("\nuse rt_spi_transfer_message() read w25q ID is:%x%x\n", id[3], id[4]);
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    rt_thread_init(&thread, "ccm3310", ccm3310_test, RT_NULL, thread_stack, sizeof(thread_stack), 20, 15);
    // rt_thread_startup(&thread);

    spi_w25q();
    while (1) {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
