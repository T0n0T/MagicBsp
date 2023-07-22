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
#include <ccm3310.h>

/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

struct rt_spi_device *spi_dev_w25q;
#define SPI_CS GET_PIN(C, 13)

void spi_w25q(void)
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
    cfg.max_hz     = 1 * 1000 * 1000;
    rt_spi_configure(spi_dev_w25q, &cfg);
    while (1) {
        struct rt_spi_message msg;
        {

            msg.send_buf   = &w25x_read_id;
            msg.recv_buf   = RT_NULL;
            msg.length     = 1;
            msg.cs_take    = 1;
            msg.cs_release = 0;
            msg.next       = RT_NULL;
            rt_spi_transfer_message(spi_dev_w25q, &msg);

            msg.send_buf   = RT_NULL;
            msg.recv_buf   = id;
            msg.length     = 5;
            msg.cs_take    = 0;
            msg.cs_release = 1;
            msg.next       = RT_NULL;

            rt_spi_transfer_message(spi_dev_w25q, &msg);
            rt_kprintf("\nuse rt_spi_transfer_message() read w25q ID is:%x%x\n", id[3], id[4]);
        }
    }
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    ccm3310_init();

    while (1) {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
