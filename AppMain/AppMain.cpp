#include <string.h>

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "cmsis_os2.h"

#include "lwip/netif.h"

#include "Network.h"

#define MQTT_MAX_TOPIC_ALIAS 100

typedef struct {
	uint8_t topic[MQTT_MAX_TOPIC_ALIAS];
	uint16_t last_len;
	uint8_t data[MQTT_OUTPUT_RINGBUF_SIZE];
	uint16_t data_len;
} msg_t;

msg_t buffer[100] = {};

void MainThr(__attribute__((unused)) void *arg);
void net_connect_cb(Network *pnet, mqtt_connection_status_t status);
void mqtt_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
void mqtt_pub_cb(void *arg, const char *topic, u32_t tot_len);

int main(void)
{
	system_init();
	BaseType_t ret = xTaskCreate(MainThr, "MainTask", 500, NULL, 1, NULL);
	if (ret != pdPASS) {
		Error_Handler();
	}
	vTaskStartScheduler();
	Error_Handler();
}

void MainThr(__attribute__((unused)) void *arg)
{
	if (osKernelInitialize()) {
		Error_Handler();
	}
	msg_t *payload = &buffer[0];

	Network net;

	net.mqtt.setUser("Network");
	net.mqtt.setPass("Network");
	ipaddr_aton("192.168.0.1", &net.mqtt.broker);
	net.init(mqtt_pub_cb, mqtt_data_cb, net_connect_cb, &payload);

	while (1) {
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		vTaskDelay(1000);
	}
}

void net_connect_cb(Network *pnet, mqtt_connection_status_t status)
{
	static char ip[28] = { 0 };
	static char ip_wildcard[32] = { 0 };
	if (status == MQTT_CONNECT_ACCEPTED) {
		snprintf(ip, sizeof(ip), "%u.%u.%u.%u",
			 ip4_addr1(&pnet->pnetif->ip_addr),
			 ip4_addr2(&pnet->pnetif->ip_addr),
			 ip4_addr3(&pnet->pnetif->ip_addr),
			 ip4_addr4(&pnet->pnetif->ip_addr));

		snprintf(ip_wildcard, sizeof(ip_wildcard), "%s/#", ip);

		pnet->mqtt.publish(ip, "Init", sizeof("Init"), 1, 0, nullptr,
				   nullptr);
		pnet->mqtt.subscribe(ip_wildcard, 1, nullptr, nullptr);
	}
}

void mqtt_request_cb(void *arg, err_t err)
{
}

void mqtt_pub_cb(void *arg, const char *topic, u32_t tot_len)
{
	msg_t *payload = (msg_t *)arg;
	memcpy(payload->topic, topic, MQTT_MAX_TOPIC_ALIAS);
	payload->last_len = tot_len;
}

void mqtt_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	msg_t *payload = (msg_t *)arg;

	payload->last_len -= len;
	payload->data_len = len;
	if ((payload->last_len == 0) || (flags & MQTT_DATA_FLAG_LAST))
		payload++;

	if (payload >= &buffer[99]) {
		payload = &buffer[0];
	}
}
