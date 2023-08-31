/**
 * @file app.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-08-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "app.h"
#include "cJSON.h"
#include "paho_mqtt.h"
#include <rtdevice.h>
#include <stdio.h>

#define DBG_LVL DBG_INFO
#define DBG_TAG "app"
#include <rtdbg.h>

MQTTClient client;
// #define USING_HWTIMER
#define USING_SOFT_TIMER
#if defined(USING_HWTIMER)
#define TIMER_DEV "timer3"
rt_device_t timer = NULL;

static rt_err_t hwtimer_call(rt_device_t dev, rt_size_t size)
{
    paho_mqtt_publish(&client, QOS1, MQTT_TOPIC_DATA, "hello");
    LOG_I("publish msg!!!!!!!!\n");
    return 0;
}

static int timer_mission_init(void)
{
    rt_hwtimerval_t timeout_s = {5, 0};
    rt_hwtimer_mode_t mode    = HWTIMER_MODE_PERIOD;
    timer                     = rt_device_find(TIMER_DEV);
    if (timer == RT_NULL) {
        LOG_E("%s set callback failed!", TIMER_DEV);
        return -1;
    }
    if (rt_device_set_rx_indicate(timer, hwtimer_call)) {
        LOG_E("%s set callback failed!", TIMER_DEV);
        return -1;
    }
    if (rt_device_control(timer, HWTIMER_CTRL_MODE_SET, &mode) != RT_EOK) {
        LOG_E("%s set mode failed!", TIMER_DEV);
        return -1;
    }
    if (rt_device_open(timer, RT_DEVICE_OFLAG_RDWR) != RT_EOK) {
        LOG_E("open %s device failed!", TIMER_DEV);
        return -1;
    }
    if (rt_device_write(timer, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s)) {
        LOG_E("set timeout value failed\n");
        return -1;
    }

    return 0;
}

static int timer_mission_deinit(void)
{
    rt_device_close(timer);
    return 0;
}

#elif defined(USING_SOFT_TIMER)

rt_timer_t soft_timer = 0;
static void timer_call(void *parameter)
{
    paho_mqtt_publish(&client, QOS1, MQTT_TOPIC_DATA, "hello");
    LOG_I("publish msg!!!!!!!!\n");
}

static int timer_mission_init(void)
{
    rt_err_t result = 0;
    soft_timer      = rt_timer_create("mqtt_timer", timer_call, 0, rt_tick_from_millisecond(5000), RT_TIMER_FLAG_PERIODIC);
    result          = rt_timer_start(soft_timer);
    if (result != RT_EOK) {
        LOG_E("mqtt timer start fail!");
        return -1;
    }
    LOG_I("mqtt timer start success!");
    return 0;
}

static int timer_mission_deinit(void)
{
    rt_timer_stop(soft_timer);
    rt_timer_delete(soft_timer);
    return 0;
}
#endif

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
}

static int mqtt_mission_init(void)
{
    rt_memset(&client, 0, sizeof(MQTTClient));
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    client.isconnected             = 0;
    client.uri                     = MQTT_URI;

    /* config connect param */
    rt_memcpy(&client.condata, &condata, sizeof(condata));
    client.condata.clientID.cstring  = DEVICE_ID;
    client.condata.keepAliveInterval = 30;
    client.condata.cleansession      = 1;
    client.condata.username.cstring  = "test";
    client.condata.password.cstring  = "public";

    /* rt_malloc buffer. */
    client.buf_size = client.readbuf_size = 1024;
    client.buf                            = rt_calloc(1, client.buf_size);
    client.readbuf                        = rt_calloc(1, client.readbuf_size);
    if (!(client.buf && client.readbuf)) {
        LOG_E("no memory for MQTT client buffer!");
        return -1;
    }

    /* set subscribe table and event callback */
    client.messageHandlers[0].topicFilter = rt_strdup(MQTT_TOPIC_DATA);
    client.messageHandlers[0].callback    = mqtt_sub_callback;
    client.messageHandlers[0].qos         = QOS1;

    client.defaultMessageHandler = mqtt_sub_default_callback;
    if (paho_mqtt_start(&client) != RT_EOK) {
        LOG_E("MQTT client start fail!");
        return -1;
    }
    return 0;
}

void mission_init(void)
{
    int result = 0;
    result     = mqtt_mission_init();
    if (result != 0) {
        LOG_E("mqtt init fail");
    }
    result = timer_mission_init();
    if (result != 0) {
        LOG_E("hwtimer init fail");
    }
}
MSH_CMD_EXPORT(mission_init, test);

void mission_deinit(void)
{
    timer_mission_deinit();
    paho_mqtt_stop(&client);
}
MSH_CMD_EXPORT(mission_deinit, test);