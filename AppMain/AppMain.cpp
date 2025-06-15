#include <string.h>

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "cmsis_os2.h"

#include "lwip/netif.h"

#include "Logger.h"
#include "Network.h"
#include "CoreService.h"

char ip[28] = { 0 };

void MainThr(__attribute__((unused)) void *arg);
void net_connect_cb(Network *pnet, mqtt_connection_status_t status);
void mqtt_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
void mqtt_pub_cb(void *arg, const char *topic, u32_t tot_len);

void servicesCB(Services *it, const char *subName, std::span<uint8_t> data,
		void *ctx);

typedef struct {
	Services *pservice = nullptr;
	Network *pnet = nullptr;
} ServiceNetwork_t;

int main(void)
{
	system_init();

	MX_USART3_UART_Init();
	extern UART_HandleTypeDef huart3;
	logger_init(&huart3);

	BaseType_t ret = xTaskCreate(MainThr, "MainTask", 500, NULL, 1, NULL);
	if (ret != pdPASS) {
		FERROR("Failed to create MainThr task: %ld", ret);
		Error_Handler();
	}

	vTaskStartScheduler();
	FERROR("Scheduler failed to start");
	Error_Handler();
}

void MainThr(__attribute__((unused)) void *arg)
{
	if (osKernelInitialize()) {
		Error_Handler();
	}

	static Network net;
	static CoreService *core = nullptr;

	try {
		/*! @todo LWIP_ASSERT("mqtt_client_is_connected: client != NULL", client); */
		// net = new Network();
		core = new CoreService(
			Services::ServicesCB{ servicesCB, &net });
		core->init();
	} catch (const std::exception &e) {
		FERROR(e.what());
		Error_Handler();
	}

	net.mqtt.setUser("Network");
	net.mqtt.setPass("Network");
	ipaddr_aton("192.168.0.1", &net.mqtt.broker);
	if (net.init(mqtt_pub_cb, mqtt_data_cb, net_connect_cb,
		     (Services *)&core)) {
		FERROR("net init");
		Error_Handler();
	}

	while (1) {
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		vTaskDelay(1000);
	}
}

void net_connect_cb(Network *pnet, mqtt_connection_status_t status)
{
	memset(ip, 0, sizeof(ip));
	static char ip_wildcard[32] = { 0 };
	FINFO("Network mqtt status: %d", status);
	switch (status) {
	case MQTT_CONNECT_ACCEPTED:
		snprintf(ip, sizeof(ip), "%u.%u.%u.%u",
			 ip4_addr1(&pnet->pnetif->ip_addr),
			 ip4_addr2(&pnet->pnetif->ip_addr),
			 ip4_addr3(&pnet->pnetif->ip_addr),
			 ip4_addr4(&pnet->pnetif->ip_addr));

		snprintf(ip_wildcard, sizeof(ip_wildcard), "%s", ip);

		if (pnet->mqtt.publish(ip, "Init", sizeof("Init"), 1, 0,
				       nullptr, nullptr))
			FERROR("Publish");

		if (pnet->mqtt.subscribe(ip_wildcard, 1, nullptr, nullptr))
			FERROR("Subscribe");

		FINFO("Network connected: %s", ip);
		break;

	default:
		FWARNING("Network mqtt status: %d", status);
		break;
	}
}

void mqtt_request_cb(void *arg, err_t err)
{
	ServiceNetwork_t *context = static_cast<ServiceNetwork_t *>(arg);

	if (err)
		FERROR("err::%d", err);

	if (context) {
		if ((context->pnet) && (context->pservice)) {
			TaskHandle_t taskHandle = nullptr;
			context->pservice->getHWParam(
				CoreServiceHW::CORE_HW_PARAM_TASK_HANDLE,
				(void **)&taskHandle);

			err == ERR_OK ? ++context->pservice->stat.msgOK :
					++context->pservice->stat.msgERR;

			if (taskHandle)
				context->pnet->unLock(taskHandle);

			FINFO("status::%s::%d", context->pservice->getName(),
			      err);
		}
	}
}

void mqtt_pub_cb(void *arg, const char *topic, u32_t tot_len)
{
	FINFO("%s\ttot_len::%lu", topic, tot_len);
}

void mqtt_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	auto *service = static_cast<Services *>(arg);
	if (service == nullptr) {
		FERROR("Service handler::nullptr");
		return;
	}

	lwjsonr_t res = service->parcer(data, len);
	switch (res) {
	case lwjsonSTREAMDONE:
		break;
	case lwjsonSTREAMINPROG:
		break;
	default:
		FWARNING("res::%d", res);
		break;
	}
}

void servicesCB(Services *it, const char *subName, std::span<uint8_t> payload,
		void *ctx)
{
	Network *pnet = static_cast<Network *>(ctx);
	std::array<char, 256> topic = { 0 };
	mqtt_connection_status_t st = MQTT_CONNECT_DISCONNECTED;

	if (pnet == nullptr) {
		FERROR("pnet::nullptr");
		return;
	}

	ServiceNetwork_t context{ it, pnet };

	if ((st = pnet->mqtt.getStatus()) != MQTT_CONNECT_ACCEPTED) {
		FERROR("pnet::mqtt:status %d", st);
		return;
	}

	snprintf(topic.data(), topic.size(), "%s/%s/%s", ip, it->getName(),
		 subName);

	pnet->mqtt.publish(topic.data(), payload.data(), payload.size(), 1, 0,
			   mqtt_request_cb, &context);

	pnet->lock(portMAX_DELAY);
}
