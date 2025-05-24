#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "Network.h"
#include "lwip.h"
#include "lwipEvents.h"

enum NET_EVENT_FLAG {
	MQTT_EVENT_LINK_UP = (EventBits_t)(1 << 3),
	MQTT_EVENT_LINK_DOWN = (EventBits_t)(1 << 4),
	MQTT_EVENT_ERRORS = (EventBits_t)(1 << 5),
	NETWORK_EVENTS_ALL =
		(EventBits_t)(LWIP_EVENT_FLAG_LINK_UP |
			      LWIP_EVENT_FLAG_LINK_DOWN |
			      LWIP_EVENT_FLAG_LINK_DHCP | MQTT_EVENT_LINK_UP |
			      MQTT_EVENT_LINK_DOWN | MQTT_EVENT_ERRORS),
	NETWORK_EVENTS_COUNTER = ((sizeof(uint32_t) << 3) - 8),
};

extern struct netif gnetif;

void mqtt_connect_cb(mqtt_client_t *client, void *arg,
		     mqtt_connection_status_t status);

Network::Network(void)
{
	pnetif = &gnetif;
	MX_ETH_Init();
}

Network::~Network(void)
{
	HAL_ETH_DeInit(&heth);
	if (taskHandle != NULL) {
		vTaskDelete(taskHandle);
	}
}

void NetworkThr(void *arg)
{
	static_cast<Network *>(arg)->eventsThr();
}

int32_t Network::init(mqtt_incoming_publish_cb_t pub_cb,
		      mqtt_incoming_data_cb_t data_cb,
		      net_connect_cb_t pconnect_cb, void *arg)
{
	int32_t ret = -1;
	do {
		if ((ret = MX_LWIP_Init()))
			break;

		connect_cb = pconnect_cb;

		if ((ret = mqtt.init(pub_cb, data_cb, arg)))
			break;

		ret = -1;
		if (pdPASS != xTaskCreate(NetworkThr, xNetTaskName, stackSize,
					  this, priority, &taskHandle))
			break;
		ret = 0;
	} while (0);

	return ret;
}

int Network::eventsThr(void)
{
	EventBits_t Event = 0;
	EventBits_t Mask = 1;
	while (1) {
		Event = xEventGroupWaitBits(lwip_event_group,
					    NETWORK_EVENTS_ALL, pdFALSE,
					    pdFALSE, portMAX_DELAY);
		Mask = 1;
		for (uint8_t i = 0; i < NETWORK_EVENTS_COUNTER; i++) {
			if (Event & Mask) {
				if (this->eventHandle(Event & Mask)) {
					/*! \todo add logs */
					Error_Handler();
				}
			}
			Mask <<= 1;
		}
	}
	return 0;
}

int Network::eventHandle(EventBits_t Event)
{
	xEventGroupClearBits(lwip_event_group, Event);

	switch (Event) {
	case LWIP_EVENT_FLAG_LINK_DOWN:
		mqtt.disconnect();
		dhcp_stop(&gnetif);
		break;
	case MQTT_EVENT_LINK_UP:
		break;
	case MQTT_EVENT_LINK_DOWN:
		break;
	case MQTT_EVENT_ERRORS:
		mqtt.disconnect();
		break;
	case LWIP_EVENT_FLAG_LINK_UP:
		dhcp_start(&gnetif);
	case LWIP_EVENT_FLAG_LINK_DHCP:
		mqtt.disconnect();
		configDevID(&gnetif.ip_addr);

		mqtt.connect(&mqtt.broker, MQTT_PORT, mqtt_connect_cb,
			     static_cast<void *>(this));
		break;

	default:
		return -1;
	}
	return 0;
}

void Network::configDevID(ip4_addr_t *ip)
{
	static char dev[28] = { 0 };

	snprintf(dev, sizeof(dev), "dev_%u.%u.%u.%u", ip4_addr1(ip),
		 ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));

	mqtt.setClientId(dev);
}

void mqtt_connect_cb(mqtt_client_t *client, void *arg,
		     mqtt_connection_status_t status)
{
	Network *pnet = static_cast<Network *>(arg);
	pnet->mqtt.setStatus(status);
	switch (status) {
	case MQTT_CONNECT_ACCEPTED:
		osEventFlagsSet(lwip_event_group, MQTT_EVENT_LINK_UP);
		break;
	case MQTT_CONNECT_DISCONNECTED:
		osEventFlagsSet(lwip_event_group, MQTT_EVENT_LINK_DOWN);
		break;
	default:
		osEventFlagsSet(lwip_event_group, MQTT_EVENT_ERRORS);
		break;
	}
	if (pnet->connect_cb)
		pnet->connect_cb(pnet, status);
}
