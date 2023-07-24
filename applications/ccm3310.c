/**
 * @file ccm3310.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <ccm3310.h>

static struct rt_thread thread;
static char thread_stack[1024];
struct rt_spi_device ccm;

uint8_t Get_Version[] = {
    0x53, 0x02, 0x10, 0x33,
    0x50, 0x00, 0x00, 0x00,
    0x80, 0x30, 0x00, 0x00,
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x02, 0x33, 0x01};

uint8_t SM2_Gen_Key[] = {
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

uint8_t SM2_Export_Key[] = {
    // 1.包头
    0x53, 0x02, 0x10, 0x33,
    // 2.数据区长度
    0x04, 0x00, 0x00, 0x00,
    // 3.0x5e导出sm2密钥，
    0x80, 0x5E, 0x00, 0x00,
    // 4.保留
    0x55, 0x55, 0x55, 0x55,
    // 5.数据区
    // 0x00结构体版本，0x00密钥id，导出sram中密钥，0x01导出公钥，保留
    0x00, 0x00, 0x01, 0x00,
    // 6.包尾
    0x55, 0x02, 0x33, 0x01};

uint8_t Import_Sym_Key[] = {
    // 1.包头
    0x53, 0x02, 0x10, 0x33,
    // 2.数据区长度
    0x14, 0x00, 0x00, 0x00,
    // 3.0x42导入sm4会话密钥，0x00新增，0x00保留在sram，掉电复位
    0x80, 0x42, 0x00, 0x00,
    // 4.保留
    0x55, 0x55, 0x55, 0x55,
    // 5.数据区
    // 0x00结构体版本，0x00新增密钥不适用，0x84算法sm4，sm4密钥长度16=0x10
    0x00, 0x00, 0x84, 0x10,
    // 密钥
    0x77, 0x7f, 0x23, 0xc6,
    0xfe, 0x7b, 0x48, 0x73,
    0xdd, 0x59, 0x5c, 0xff,
    0xf6, 0x5f, 0x58, 0xec,
    // 6.包尾
    0x55, 0x02, 0x33, 0x01};

uint8_t Sym_Encrypt[] = {
    // 1.包头
    0x53, 0x02, 0x10, 0x33,
    // 2.数据区长度
    0x30, 0x00, 0x00, 0x00,
    // 3.0x44导入sm4加密，新增，保留在sram，掉电复位
    0x80, 0x44, 0x00, 0x00,
    // 4.保留
    0x55, 0x55, 0x55, 0x55,
    // 5.数据区
    // 5.1 0x00结构体版本，
    // 5.2 密钥id num.17，
    // 5.3 0x00算法ecb，
    // 5.4 sm1相关，
    // 5.5 des/aes相关，
    // 5.6 0x01分组长度128bit,
    // 5.7不使用初始向量，
    // 5.8保留，
    // (5.9无初始向量置空)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    // 5.10明文
    0x68, 0x65, 0x6c, 0x6c,
    0x6f, 0x21, 0x68, 0x65,
    0x72, 0x65, 0x27, 0x73,
    0x20, 0x61, 0x20, 0x74,
    0x65, 0x73, 0x74, 0x2c,
    0x77, 0x69, 0x74, 0x68,
    0x20, 0x75, 0x74, 0x66,
    0x2d, 0x38, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    // 6.包尾
    0x55, 0x02, 0x33, 0x01};

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

static void ccm3310_transfer(uint8_t *send_buf, uint8_t *recv_buf, int length)
{
    struct rt_spi_message msg;
    rt_int8_t status = PIN_HIGH;
    rt_pin_write(GINT0, PIN_LOW);
    while (status == PIN_HIGH) {
        status = rt_pin_read(GINT1);
    }
    msg.send_buf   = send_buf;
    msg.recv_buf   = RT_NULL;
    msg.length     = sizeof(send_buf);
    msg.cs_take    = 1;
    msg.cs_release = 1;
    msg.next       = RT_NULL;
    rt_spi_transfer_message(&ccm, &msg);

    while (status == PIN_HIGH) {
        status = rt_pin_read(GINT1);
    }

    msg.send_buf   = RT_NULL;
    msg.recv_buf   = recv_buf;
    msg.length     = length;
    msg.cs_take    = 1;
    msg.cs_release = 1;
    msg.next       = RT_NULL;
    rt_spi_transfer_message(&ccm, &msg);
}

static void ccm3310_version(void)
{
    uint8_t r[100]   = {0xff};
    int len          = 0;
    uint8_t *version = 0;

    ccm3310_transfer(Get_Version, r, sizeof(r));
    printf("==========print version ==========\n");
    for (size_t i = 0; i < sizeof(r); i++) {
        printf("0x%02x ", r[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
    }
    int err = decode(r, &version, &len);
    if (err == 0) {
        printf("%s\n", version);
    }

    printf("\n=======================\n");
}
MSH_CMD_EXPORT(ccm3310_version, get version);

static void ccm3310_sm2_exportkey(void)
{
    uint8_t r[100] = {0xff};
    int len        = 0;
    uint8_t *id, *sm2_key;

    ccm3310_transfer(SM2_Gen_Key, r, 21);
    printf("==========print key id ==========\n");
    for (size_t i = 0; i < sizeof(r); i++) {
        printf("0x%02x ", r[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
        if (r[i] == 0xff) break;
    }
    int err = decode(r, &id, &len);
    if (err == 0) {
        printf("%d\n", *id);
    }
    printf("\n=======================\n");
}

void ccm3310_thread_start(void)
{
    // rt_thread_init(&thread, "ccm3310", ccm3310_entry, RT_NULL, thread_stack, sizeof(thread_stack), 20, 15);
    // rt_thread_startup(&thread);
}