/**
 * @file rand_rtt.c
 * @author T0n0T (T0n0T@823478402@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmssl/rand.h>
#include <gmssl/error.h>

#define RAND_MAX_BUF_SIZE 4096

int rand_bytes(uint8_t *buf, size_t len)
{
    if (!buf) {
        error_print();
        return -1;
    }
    if (len > RAND_MAX_BUF_SIZE) {
        error_print();
        return -1;
    }
    if (!len) {
        return 0;
    }
    for (size_t i = 0; i < len; i++) {
        buf[i] = i;
    }

    return 1;
}