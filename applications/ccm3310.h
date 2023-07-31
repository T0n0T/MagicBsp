/**
 * @file ccm3310.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-24
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "drv_gpio.h"
#include "drv_spi.h"
#include <stdio.h>

#define CS_PIN GET_PIN(B, 12)

#define POR    GET_PIN(C, 7)
#define GINT0  GET_PIN(C, 8)
#define GINT1  GET_PIN(C, 9)

struct ccm3310_key_data {
    uint8_t version;
    uint8_t key_id; /*新增写0x00*/
    uint8_t algo;   /*算法标识，0x81(DES)、0x82(AES)、 0x83(SM1)、0x84(SM4)*/
    uint8_t len;    /*密钥长度*/
    uint8_t key_data[16];
};

extern void ccm3310_init(void);
extern void ccm3310_thread_start(void);
int ccm3310_transfer(uint8_t *send_buf, int send_len, uint8_t **decode_data, int recv_len);
int decode(uint8_t *raw, uint8_t **data, int *len);
int encode(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *send_pack, uint8_t *data, uint32_t data_len);
uint32_t crc32(const uint8_t *buf, uint32_t size);