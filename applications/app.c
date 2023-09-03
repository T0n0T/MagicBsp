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
#include "mqttclient.h"
#include <rtdevice.h>
#include <stdio.h>

#define DBG_LVL DBG_INFO
#define DBG_TAG "app"
#include <rtdbg.h>

#define MQTT_DELAY_MS 5000

uint8_t thread_isinit = 0;
mqtt_client_t *client = NULL;
struct rt_thread mqtt_thread;
char stack[2048];
mqtt_message_t msg;

static void mqtt_entry(void *p)
{
    memset(&msg, 0, sizeof(msg));
    mqtt_error_t err = KAWAII_MQTT_SUCCESS_ERROR;
    msg.qos          = QOS0;
    msg.payload      = (void *)"this is a kawaii mqtt test ...";
    while (1) {
        // err = mqtt_publish(client, "/topic/test", &msg);
        if (err != KAWAII_MQTT_SUCCESS_ERROR) {
            LOG_E("publish msg fail, err[%d]", err);
        }
        LOG_D("publish msg success!!!!!!!!\n");
        rt_thread_mdelay(MQTT_DELAY_MS);
    }
}

static int thread_mission_init(void)
{
    rt_err_t err = RT_EOK;
    // memset(stack, 0, sizeof(stack));
    if (!thread_isinit) {
        err = rt_thread_init(&mqtt_thread, "mqtt", mqtt_entry, RT_NULL, stack, sizeof(stack), 20, 10);
        if (err != RT_EOK) {
            LOG_E("mqtt thread init fail, err[%d]", err);
            return err;
        }
        thread_isinit = 1;
    }
    return rt_thread_startup(&mqtt_thread);
}

static void thread_mission_deinit(void)
{
    rt_thread_detach(&mqtt_thread);
}

static void sub_handle(void *client, message_data_t *msg)
{
    (void)client;
    LOG_I("-----------------------------------------------------------------------------------");
    LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char *)msg->message->payload);
    LOG_I("-----------------------------------------------------------------------------------");
}

static int mqtt_mission_init(void)
{

    mqtt_error_t err = KAWAII_MQTT_SUCCESS_ERROR;
    rt_thread_delay(2000);

    // mqtt_log_init();

    client = mqtt_lease();
    if (client == RT_NULL) {
        LOG_E("mqtt alloec fail");
        return -1;
    }

    mqtt_set_host(client, MQTT_URI_HOST);
    mqtt_set_port(client, MQTT_URI_PORT);
    // mqtt_set_user_name(client, "rt-thread");
    // mqtt_set_password(client, "rt-thread");
    mqtt_set_client_id(client, DEVICE_ID);
    mqtt_set_clean_session(client, 1);

    KAWAII_MQTT_LOG_I("The ID of the Kawaii client is: %s ", DEVICE_ID);

    err = mqtt_connect(client);
    if (err != KAWAII_MQTT_SUCCESS_ERROR) {
        LOG_E("mqtt connect fail, err[%d]", err);
        return -1;
    }
    err = mqtt_subscribe(client, "/topic/test", QOS1, sub_handle);
    if (err != KAWAII_MQTT_SUCCESS_ERROR) {
        LOG_E("mqtt set subscribe fail, err[%d]", err);
        return -1;
    }
    return 0;
}

void mission_init(void)
{
    if (mqtt_mission_init() != 0) {
        return;
    }
    if (thread_mission_init() != 0) {
        mqtt_disconnect(client);
        mqtt_release(client);
        rt_free(client);
    }
}
MSH_CMD_EXPORT(mission_init, test);

void mission_deinit(void)
{
    // thread_mission_deinit();
    printf("1\n");
    mqtt_disconnect(client);
    printf("2\n");
    mqtt_release(client);
    printf("3\n");
    rt_free(client);
    printf("4\n");
}
MSH_CMD_EXPORT(mission_deinit, test);