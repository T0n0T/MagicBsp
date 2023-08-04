#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"

#define DRV_DEBUG
#define LOG_TAG "drv.i2c"
#include <drv_log.h>

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;
struct rt_i2c_bus_device i2c1_bus;

static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    /* DMA1_Channel7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

static void MX_I2C1_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = 100000;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2     = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}

static stm32_i2c_init(struct rt_i2c_bus_device *bus)
{
    MX_I2C1_Init();
    MX_DMA_Init();
}

static rt_ssize_t _master_xfer(struct rt_i2c_bus_device *bus,
                               struct rt_i2c_msg msgs[],
                               rt_uint32_t num)
{
    struct rt_i2c_msg *msg;
    HAL_StatusTypeDef ret = HAL_OK;
    rt_int32_t i          = 0;
    uint32_t option       = 0;
    for (i = 0; i < num; i++) {
        msg    = &msgs[i];
        option = I2C_FIRST_AND_LAST_FRAME;
        LOG_I("I2C1 msg[%d] to addr [0x%02X]with lenof [%d]", i, msg->addr, msg->len);
        if (msg->flags & RT_I2C_RD) {
            ret = HAL_I2C_Master_Seq_Receive_DMA(&hi2c1, msg->addr, msg->buf, msg->len, option);
            if (ret != HAL_OK) {
                goto out;
            }
        } else {
            if (msg->flags & RT_I2C_NO_READ_ACK) {
                option = I2C_LAST_FRAME_NO_STOP;
            }
            ret = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, msg->addr, msg->buf, msg->len, option);
            if (ret != HAL_OK) {
                goto out;
            }
        }
    }

out:
    LOG_E("I2C wrong with err[%d]", ret);
    return i;
}

static const struct rt_i2c_bus_device_ops _i2c_ops =
    {
        _master_xfer,
        NULL,
        NULL,
};

int rt_hw_i2c_init(void)
{
    rt_err_t result;
    stm32_i2c_init(&i2c1_bus);
    rt_memcpy(&i2c1_bus, 0, sizeof(struct rt_i2c_bus_device));
    i2c1_bus.ops     = &_i2c_ops;
    i2c1_bus.timeout = 0;
    result           = rt_i2c_bus_device_register(&i2c1_bus, "i2c1");
    RT_ASSERT(result == RT_EOK);
    return RT_EOK;
}
INIT_BOARD_EXPORT(rt_hw_i2c_init);