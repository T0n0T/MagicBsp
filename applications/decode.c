/**
 * @file decode.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-24
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <ccm3310.h>

static uint16_t decode_2bytes(uint8_t *msb)
{
    return *msb | (*(msb + 1) << 8);
}

static uint32_t decode_4bytes(uint8_t *msb)
{
    return *msb | (*(msb + 1) << 8) | (*(msb + 2) << 16) | (*(msb + 3) << 24);
}

static void decode_test()
{
    uint8_t *data = 0;
    int len;
    uint8_t r[100] = {
        0x52, 0x02, 0x10, 0x33,
        // 数据长度
        0x50, 0x00, 0x00, 0x00,
        // 状态字
        0x00, 0x90,
        0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
        // 数据区
        0x43, 0x43, 0x4d, 0x33,
        0x33, 0x31, 0x30, 0x53,
        0x2d, 0x54, 0x20, 0x53,
        0x50, 0x49, 0x20, 0x41,
        0x4c, 0x47, 0x4f, 0x20,
        0x43, 0x4f, 0x53, 0x20,
        0x56, 0x31, 0x2e, 0x33,
        0x30, 0x20, 0x32, 0x30,
        0x31, 0x39, 0x2e, 0x31,
        0x31, 0x2e, 0x32, 0x36,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x43, 0x43, 0x4d, 0x33,
        0x33, 0x31, 0x30, 0x53,
        0x2d, 0x54, 0x20, 0x51,
        0x46, 0x4e, 0x33, 0x32,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        // 包尾
        0x56, 0x02, 0x33, 0x01};

    decode(r, &data, &len);

    printf("%d\n", len);
    printf("%p\n", data);
    printf("%s\n", data);
}
MSH_CMD_EXPORT(decode_test, decode test);

int decode(uint8_t *raw, uint8_t **data, int * len)
{
    if (decode_4bytes(&raw[0]) != 0x33100252) {
        printf("decode wrong with upheader 0x%X, 0x%X\n", decode_4bytes(&raw[0]), 0x33100252);
        return -1;
    }

    *len = (int)decode_4bytes(&raw[4]);
    if (decode_2bytes(&raw[8]) != 0x9000) {
        printf("decode wrong with status 0x%X\n", decode_4bytes(&raw[0]));
        return -2;
    }
    *data = &raw[16];
    return 0;
}

/*
    // 1.包头
    0x53, 0x02, 0x10, 0x33,
    // 2.数据区长度
    0x04, 0x00, 0x00, 0x00,
    // 3.0x5a生成sm2密钥，新增，保留在sram，掉电复位
    0x80, 0x5A, 0x00, 0x00,
    // 4.保留
    0x55, 0x55, 0x55, 0x55,
    // 5.数据区
    // 0x00结构体版本，0x00新增密钥不适用，0x00私钥可导出，保留
    0x00, 0x00, 0x84, 0x00,
    // 6.包尾
    0x55, 0x02, 0x33, 0x01};
    */
uint8_t *encode(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data)
{
    uint8_t *send_pack = (uint8_t *)rt_malloc(sizeof(data) + 30);
    uint8_t *ptr       = 0;
    ptr                = send_pack;
    // 1.包头
    *ptr     = 0x53;
    *(ptr++) = 0x02;
    *(ptr++) = 0x10;
    *(ptr++) = 0x33;
    // 2.数据区长度
    *(ptr++) = sizeof(data) & 0x000000FF;
    *(ptr++) = sizeof(data) & 0x0000FF00;
    *(ptr++) = sizeof(data) & 0x00FF0000;
    *(ptr++) = sizeof(data) & 0xFF000000;
    // 3.命令段
    *(ptr++) = cla;
    *(ptr++) = ins;
    *(ptr++) = p1;
    *(ptr++) = p2;
    // 4.保留
    *(ptr++) = 0x55;
    *(ptr++) = 0x55;
    *(ptr++) = 0x55;
    *(ptr++) = 0x55;
    // 5.数据区
    for (size_t i = 0; i < sizeof(*data); i++) {
        *(ptr++) = *(data + i);
    }
    // 6.包尾
    if (cla == 0x81) {
        uint32_t crc32_val = crc32(send_pack, ptr - send_pack + 1);
        *(ptr++)           = crc32_val & 0x000000FF;
        *(ptr++)           = crc32_val & 0x0000FF00;
        *(ptr++)           = crc32_val & 0x00FF0000;
        *(ptr++)           = crc32_val & 0xFF000000;
    }
    return send_pack;
}