#include <string.h>

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "cmsis_os2.h"

#include "lwip/netif.h"

#include "Network.h"
#include "ServicesCore.h"
#include "UserService.h"

#define MQTT_MAX_TOPIC_ALIAS 100

ServicesCore core;
char ip[28] = { 0 };
std::queue<std::span<uint8_t> > queue;

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
	static std::array<uint8_t, 256> request = {};
	UserService userService;

	try {
		core.regService();
		userService.regService();
	} catch (const std::exception &e) {
	}

	Network net;

	net.mqtt.setUser("Network");
	net.mqtt.setPass("Network");
	ipaddr_aton("192.168.0.1", &net.mqtt.broker);
	net.init(mqtt_pub_cb, mqtt_data_cb, net_connect_cb, &request);

	while (1) {
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		vTaskDelay(1000);
	}
}

void net_connect_cb(Network *pnet, mqtt_connection_status_t status)
{
	memset(ip, 0, sizeof(ip));
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
	auto *request = static_cast<std::array<uint8_t, 256> *>(arg);
	Services::head_t *head =
		reinterpret_cast<Services::head_t *>(request->data());

	std::array<uint8_t, 16> topic_hash_buf = {};
	size_t topic_offset = strlen(ip) + sizeof('/');
	size_t topic_len = strlen(topic + topic_offset);

	head->len = 0;
	head->hash = 0;

	do {
		if (tot_len > request->size())
			break;

		if (strncmp(topic, ip, strlen(ip)) != 0)
			break;

		if (topic_len > topic_hash_buf.size())
			break;

		memcpy(topic_hash_buf.data(), topic + topic_offset, topic_len);

		head->len = tot_len;
		head->hash = core.getHash(topic_hash_buf);
	} while (0);
}

void mqtt_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	auto *request = static_cast<std::array<uint8_t, 256> *>(arg);
	Services::head_t *head =
		reinterpret_cast<Services::head_t *>(request->data());

	if (head->len == 0 || head->hash == 0 ||
	    len > (request->size() -
		   (sizeof(head->hash) + sizeof(head->len)))) {
		return;
	}
	uint8_t *ptr = static_cast<uint8_t *>(
		(request->data()) + sizeof(head->hash) + sizeof(head->len));
	memcpy(ptr, data, len);
	head->len += sizeof(head->hash) + sizeof(head->len);

	try {
		core.callService(std::span<uint8_t>(request->data(),
						    head->len +
							    sizeof(head->hash) +
							    sizeof(head->len)),
				 queue);
	} catch (const std::exception &e) {
	}
}
