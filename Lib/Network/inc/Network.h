#pragma once
#include "mqttClient.h"
#include "semphr.h"
class Network {
    private:
	TaskHandle_t taskHandle;
	const StackType_t stackSize = (configMINIMAL_STACK_SIZE << 3);
	const UBaseType_t priority = osPriorityRealtime7;
	const char *xNetTaskName = "Network";
	void configDevID(ip4_addr_t *ip);

    public:
	typedef void (*net_connect_cb_t)(Network *pnet,
					 mqtt_connection_status_t status);
	struct netif *pnetif;
	net_connect_cb_t connect_cb;
	mqttClient mqtt;

	int32_t init(mqtt_incoming_publish_cb_t pub_cb,
		     mqtt_incoming_data_cb_t data_cb,
		     net_connect_cb_t connect_cb, void *arg);

	void lock(uint32_t timeout);
	void unLock(TaskHandle_t TaskAHandle);
	int eventHandle(EventBits_t Event);
	int eventsThr(void);

	Network(void);
	~Network(void);
};
