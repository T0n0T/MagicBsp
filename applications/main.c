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

#define POR    GET_PIN(C, 7)
#define GINT0  GET_PIN(C, 8)
#define GINT1  GET_PIN(C, 9)

uint8_t r1[] = {
    0x52, 0x02, 0x10, 0x33,
    0x00, 0x00, 0x00, 0x00,
    0x97, 0x34, 0x5a, 0x5a,
    0x5a, 0x5a, 0x5a, 0x5a,
    0x56, 0x02, 0x33, 0x01,
    0x15, 0x85, 0xa5, 0xa5,
    0xa5, 0xa5, 0xa5, 0xa5,
    0xa5, 0xa5, 0xa5, 0xa5,
    0xa5, 0xa5, 0xa5, 0xa5,
    0xa5, 0xa5, 0x50, 0x23,
    0x30, 0x1f};

uint8_t r2[] = {
    0x52, 0x02, 0x10, 0x33,
    0x00, 0x00, 0x00, 0x00,
    0x97, 0x34,
    0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
    0x56, 0x02, 0x33, 0x01,
    0x15, 0x82, 0xd2, 0xd2,
    0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2,
    0xa8, 0x11, 0x98, 0x0f};

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
    cfg.max_hz     = 1 * 1000 * 1000;
    rt_spi_configure(&ccm, &cfg);
    rt_pin_write(POR, PIN_LOW);
    HAL_Delay(20);
    rt_pin_write(POR, PIN_HIGH);
}

void ccm3310_test(void *p)
{
    uint8_t data[20] = {
        0x55, 0x02, 0x10, 0x33,
        0x50, 0x00, 0x00, 0x00,
        0x80, 0x30, 0x00, 0x00,
        0x55, 0x55, 0x55, 0x55,
        0x55, 0x02, 0x33, 0x01};
    uint8_t r[100] = {0xff};
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

    struct rt_spi_message send_msg1, send_msg2;
    struct rt_spi_message recv_msg1, recv_msg2, recv_msg3, recv_msg4, recv_msg5, recv_msg6, recv_msg7;
    rt_int8_t status = PIN_HIGH;

    while (1) {
        memset(r, 0xff, sizeof(r));
        printf("\n=======================\n");

        send_msg1.send_buf   = data;
        send_msg1.recv_buf   = RT_NULL;
        send_msg1.length     = 16;
        send_msg1.cs_take    = 1;
        send_msg1.cs_release = 0;
        send_msg1.next       = &send_msg2;

        send_msg2.send_buf   = &data[16];
        send_msg2.recv_buf   = RT_NULL;
        send_msg2.length     = 4;
        send_msg2.cs_take    = 0;
        send_msg2.cs_release = 1;
        send_msg2.next       = RT_NULL;
        rt_pin_write(GINT0, PIN_LOW);
        status = PIN_HIGH;
        while (status == PIN_HIGH) {
            status = rt_pin_read(GINT1);
            // printf("status: 0x%2x\n", status);
        }
        printf("==========begin transmit==========\n");

        rt_spi_transfer_message(&ccm, &send_msg1);

        printf("==========print transmit ==========\n");
        for (size_t i = 0; i < sizeof(data); i++) {
            printf("0x%02x ", r[i]);
            if (i % 4 == 3) {
                printf("\n");
            }
        }
        status = PIN_HIGH;
        while (status == PIN_HIGH) {
            status = rt_pin_read(GINT1);
            // printf("status: 0x%2x\n", status);
        }
        memset(r, 0xff, sizeof(r));
        recv_msg1.send_buf   = RT_NULL;
        recv_msg1.recv_buf   = r;
        recv_msg1.length     = 16;
        recv_msg1.cs_take    = 1;
        recv_msg1.cs_release = 0;
        recv_msg1.next       = &recv_msg2;

        recv_msg2.send_buf   = RT_NULL;
        recv_msg2.recv_buf   = &r[16];
        recv_msg2.length     = 16;
        recv_msg2.cs_take    = 0;
        recv_msg2.cs_release = 0;
        recv_msg2.next       = &recv_msg3;

        recv_msg3.send_buf   = RT_NULL;
        recv_msg3.recv_buf   = &r[32];
        recv_msg3.length     = 16;
        recv_msg3.cs_take    = 0;
        recv_msg3.cs_release = 0;
        recv_msg3.next       = &recv_msg4;

        recv_msg4.send_buf   = RT_NULL;
        recv_msg4.recv_buf   = &r[48];
        recv_msg4.length     = 16;
        recv_msg4.cs_take    = 0;
        recv_msg4.cs_release = 0;
        recv_msg4.next       = &recv_msg5;

        recv_msg5.send_buf   = RT_NULL;
        recv_msg5.recv_buf   = &r[64];
        recv_msg5.length     = 16;
        recv_msg5.cs_take    = 0;
        recv_msg5.cs_release = 0;
        recv_msg5.next       = &recv_msg6;

        recv_msg6.send_buf   = RT_NULL;
        recv_msg6.recv_buf   = &r[80];
        recv_msg6.length     = 16;
        recv_msg6.cs_take    = 0;
        recv_msg6.cs_release = 0;
        recv_msg6.next       = &recv_msg7;

        recv_msg7.send_buf   = RT_NULL;
        recv_msg7.recv_buf   = &r[96];
        recv_msg7.length     = 4;
        recv_msg7.cs_take    = 0;
        recv_msg7.cs_release = 1;
        recv_msg7.next       = RT_NULL;

        printf("==========begin receive ==========\n");

        rt_spi_transfer_message(&ccm, &recv_msg1);

        printf("==========print recvive ==========\n");
        for (size_t i = 0; i < sizeof(r); i++) {
            printf("0x%02x ", r[i]);
            if (i % 4 == 3) {
                printf("\n");
            }
        }
        printf("\n=======================\n");

        rt_thread_mdelay(500);
    }
}
MSH_CMD_EXPORT(ccm3310_test, test);

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

// void recv_test(void *p)
// {
//     struct rt_spi_device *spi3 = rt_malloc(sizeof(struct rt_spi_device));
//     rt_err_t err               = rt_spi_bus_attach_device_cspin(spi3, "revv", "spi3", GET_PIN(D, 2), RT_NULL);
//     if (err != RT_NULL) {
//         printf("Fail to attach %s creating spi_device %s failed.\n", "spi3", "revv");
//         return;
//     }
//     struct stm32_spi *spi_drv     = rt_container_of(spi3->bus, struct stm32_spi, spi_bus);
//     SPI_HandleTypeDef *spi_handle = &spi_drv->handle;
//     struct rt_spi_configuration cfg;
//     cfg.data_width = 8;
//     cfg.mode       = RT_SPI_SLAVE | RT_SPI_MODE_3 | RT_SPI_MSB;
//     cfg.max_hz     = 1 * 1000 * 1000;
//     rt_spi_configure(spi3, &cfg);
//     uint8_t r[100] = {0};
//     struct rt_spi_message msg;
//     while (1) {
//         msg.send_buf   = data;
//         msg.recv_buf   = RT_NULL;
//         msg.length     = sizeof(data);
//         msg.cs_take    = 1;
//         msg.cs_release = 0;
//         msg.next       = RT_NULL;
//         printf("==========begin transmit==========\n");
//         rt_spi_transfer_message(&ccm, &msg);
//         printf("==========print transmit ==========\n");
//         for (size_t i = 0; i < sizeof(data); i++) {
//             printf("0x%02x ", data[i]);
//             if (i % 4 == 3) {
//                 printf("\n");
//             }
//         }
//         printf("==========begin receive ==========\n");
//         HAL_SPI_Receive(spi_handle, r, 100, 1000);
//         printf("==========print recvive ==========\n");
//         for (size_t i = 0; i < sizeof(r); i++) {
//             printf("0x%02x ", r[i]);
//             if (i % 4 == 3) {
//                 printf("\n");
//             }
//         }
//         printf("\n=======================\n");
//     }
// }

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_thread_init(&thread, "ccm3310", ccm3310_test, RT_NULL, thread_stack, sizeof(thread_stack), 20, 15);
    ccm3310_init();

    rt_thread_startup(&thread);

    rt_thread_t tt = rt_thread_create("recv", spi_w25q, 0, 1024, 20, 15);
    // rt_thread_startup(tt);

    while (1) {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
