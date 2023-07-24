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

void ccm3310_init(void);
void ccm3310_thread_start(void);
int decode(uint8_t *raw, uint8_t **data, int *len);
uint8_t *encode(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data);
uint32_t crc32(const uint8_t *buf, uint32_t size);