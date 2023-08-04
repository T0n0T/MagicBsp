#include <rtthread.h>
#include "rtdevice.h"
#include <rthw.h>
#include <drv_common.h>

struct stm32_i2c_config {
    I2C_TypeDef *Instance;
    char *bus_name;
    IRQn_Type irq_type;
};

struct stm32_i2c {
    rt_uint32_t pin;
    char *bus_name;
    char *device_name;
};

#define I2C1_BUS_CONFIG     \
    {                       \
        .Instance = I2C1,   \
        .bus_name = "i2c1", \
    }