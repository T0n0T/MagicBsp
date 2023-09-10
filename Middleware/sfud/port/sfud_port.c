/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include <main.h>

static char log_buf[256];

typedef struct {
    SPI_TypeDef *spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
} spi_user_data, *spi_user_data_t;
static spi_user_data spi1 = {.spix = SPI1, .cs_gpiox = GPIOC, .cs_gpio_pin = LL_GPIO_PIN_13};

void sfud_log_debug(const char *file, const long line, const char *format, ...);

static void spi_lock(const sfud_spi *spi)
{
    __disable_irq();
}

static void spi_unlock(const sfud_spi *spi)
{
    __enable_irq();
}

static void retry_delay_100us(void)
{
    /* 100 microsecond delay */
    uint32_t told, tnow, tcnt = 0;
    uint32_t ticks = 100 * SysTick->LOAD;
    told           = SysTick->VAL;
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += SysTick->LOAD - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                break;
            }
        }
    }
}

static int spi_transmit_receive(uint8_t data_in, uint8_t *data_out)
{
    int state = 0;
    *data_out = 0;
    uint32_t timeout_cnt;
    static const uint32_t timeout_cnt_num = 10000;

    // Wait until TXE flag is set to send data
    timeout_cnt = 0;
    while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {
        timeout_cnt++;
        if (timeout_cnt > timeout_cnt_num) {
            state = -1;
            break;
        }
    }

    LL_SPI_TransmitData8(SPI1, data_in);

    // Check BSY flag
    timeout_cnt = 0;
    while (LL_SPI_IsActiveFlag_BSY(SPI1)) {
        timeout_cnt++;
        if (timeout_cnt > timeout_cnt_num) {
            state = -1;
            break;
        }
    }

    // Check RXNE flag
    timeout_cnt = 0;
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1)) {
        timeout_cnt++;
        if (timeout_cnt > timeout_cnt_num) {
            state = -1;
            break;
        }
    }

    // Read 16-Bits in the data register
    *data_out = LL_SPI_ReceiveData8(SPI1);

    return state;
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size)
{
    sfud_err result = SFUD_SUCCESS;
    uint8_t send_data, read_data;
    spi_user_data_t spi_dev = (spi_user_data_t)spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

    LL_GPIO_ResetOutputPin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);
    /* 开始读写数据 */
    for (size_t i = 0, retry_times; i < write_size + read_size; i++) {
        /* 先写缓冲区中的数据到 SPI 总线，数据写完后，再写 dummy(0xFF) 到 SPI 总线 */
        if (i < write_size) {
            send_data = *write_buf++;
        } else {
            send_data = SFUD_DUMMY_DATA;
        }
        /* 发送数据 */
        retry_times = 1000;
        while (!LL_SPI_IsActiveFlag_TXE(spi_dev->spix)) {
            SFUD_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != SFUD_SUCCESS) {
            goto exit;
        }
        LL_SPI_TransmitData8(spi_dev->spix, send_data);

        retry_times = 1000;
        while (LL_SPI_IsActiveFlag_BSY(spi_dev->spix)) {
            SFUD_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != SFUD_SUCCESS) {
            goto exit;
        }

        /* 接收数据 */
        retry_times = 1000;
        while (!LL_SPI_IsActiveFlag_RXNE(spi_dev->spix)) {
            SFUD_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != SFUD_SUCCESS) {
            goto exit;
        }
        read_data = LL_SPI_ReceiveData8(spi_dev->spix);
        /* 写缓冲区中的数据发完后，再读取 SPI 总线中的数据到读缓冲区 */
        if (i >= write_size) {
            *read_buf++ = read_data;
        }
    }

exit:
    LL_GPIO_SetOutputPin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);

    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size)
{
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */

    return result;
}
#endif /* SFUD_USING_QSPI */

sfud_err sfud_spi_port_init(sfud_flash *flash)
{
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    flash->spi.wr        = spi_write_read;
    flash->spi.lock      = spi_lock;
    flash->spi.unlock    = spi_unlock;
    flash->retry.delay   = retry_delay_100us;
    flash->retry.times   = 60 * 10000;
    flash->spi.user_data = &spi1;
    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}
