#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <ccm3310.h>

static struct rt_thread thread;
static char thread_stack[1024];
struct rt_spi_device ccm;

uint8_t Get_Version[20] = {
    0x55, 0x02, 0x10, 0x33,
    0x50, 0x00, 0x00, 0x00,
    0x80, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    // 0x55, 0x55, 0x55, 0x55,
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

void ccm3310_entry(void *p)
{
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

    struct rt_spi_message send_msg1, send_msg2;
    struct rt_spi_message recv_msg1, recv_msg2, recv_msg3, recv_msg4, recv_msg5, recv_msg6, recv_msg7;
    rt_int8_t status = PIN_HIGH;

    while (1) {
        memset(r, 0xff, sizeof(r));
        printf("\n=======================\n");

        send_msg1.send_buf   = Get_Version;
        send_msg1.recv_buf   = RT_NULL;
        send_msg1.length     = 16;
        send_msg1.cs_take    = 1;
        send_msg1.cs_release = 0;
        send_msg1.next       = &send_msg2;

        send_msg2.send_buf   = &Get_Version[16];
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
        for (size_t i = 0; i < sizeof(Get_Version); i++) {
            printf("0x%02x ", Get_Version[i]);
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

static void ccm3310_test(void)
{
    struct rt_spi_message msg;
    rt_int8_t status = PIN_HIGH;
    uint8_t r[100]   = {0xff};

    rt_pin_write(GINT0, PIN_LOW);
    while (status == PIN_HIGH) {
        status = rt_pin_read(GINT1);
    }

    msg.send_buf   = Get_Version;
    msg.recv_buf   = RT_NULL;
    msg.length     = sizeof(Get_Version);
    msg.cs_take    = 1;
    msg.cs_release = 1;
    msg.next       = RT_NULL;
    rt_spi_transfer_message(&ccm, &msg);

    while (status == PIN_HIGH) {
        status = rt_pin_read(GINT1);
    }

    msg.send_buf   = RT_NULL;
    msg.recv_buf   = r;
    msg.length     = sizeof(r);
    msg.cs_take    = 1;
    msg.cs_release = 1;
    msg.next       = RT_NULL;
    rt_spi_transfer_message(&ccm, &msg);

    printf("==========print recvive ==========\n");
    for (size_t i = 0; i < sizeof(r); i++) {
        printf("0x%02x ", r[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
    }
    printf("\n=======================\n");
}
MSH_CMD_EXPORT(ccm3310_test, test);

void ccm3310_thread_start(void)
{
    rt_thread_init(&thread, "ccm3310", ccm3310_entry, RT_NULL, thread_stack, sizeof(thread_stack), 20, 15);
    rt_thread_startup(&thread);
}