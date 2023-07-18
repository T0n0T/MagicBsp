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
void ccm3310_test(void *p)
{
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
    // uint8_t data[20] = {
    //     0x55,
    //     0x02,
    //     0x10,
    //     0x33,
    //     0x50,
    //     0x00,
    //     0x00,
    //     0x00,
    //     0x00,
    //     0x00,
    //     0x30,
    //     0x80,
    //     0x55,
    //     0x55,
    //     0x55,
    //     0x55,
    //     0x55,
    //     0x02,
    //     0x33,
    //     0x01,
    // };
    // uint8_t data[20] = {0x55, 0x02, 0x33, 0x01, 0x55, 0x55, 0x55, 0x55, 0x00,0x00, 0x30, 0x80, 0x50, 0x00, 0x00, 0x00, 0x53, 0x02, 0x10, 0x33};
    uint8_t data[20] = {0x33, 0x10, 0x02, 0x53, 0x00, 0x00, 0x00, 0x50, 0x80, 0x30, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x01, 0x33, 0x02, 0x55};
    uint8_t r[20] = {0};

    printf("print berfore\n");
    for (size_t i = 0; i < 20; i++) {
        printf("0x%02x ", r[i]);
    }
    printf("\n");
    rt_spi_send_then_recv(&ccm, data, 20, r, 20);

    printf("print after\n");
    for (size_t i = 0; i < 20; i++) {
        printf("0x%02x ", r[i]);
    }
    printf("\n");
}

struct rt_spi_device *spi_dev_w25q;
#define SPI_CS GET_PIN(C, 13)
void spi_w25q()
{
    rt_uint8_t w25x_read_id = 0x90;
    rt_uint8_t id[5]        = {0};
    spi_dev_w25q            = rt_malloc(sizeof(struct rt_spi_device));
    rt_err_t err = rt_spi_bus_attach_device_cspin(spi_dev_w25q, "w25q", "spi1", SPI_CS, RT_NULL);
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
    ccm3310_test(0);
    spi_w25q();
    while (1) {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
