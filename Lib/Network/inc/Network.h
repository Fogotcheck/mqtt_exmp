#pragma once

#include "lwip/apps/mqtt.h"
class Network {
    private:
	mqtt_client_t *mqtt = NULL;
	mqtt_connect_client_info_t mqtt_info = { .client_id = "Network",
						 .client_user = "Network",
						 .client_pass = "Network",
						 .keep_alive = 100,
						 .will_topic = NULL,
						 .will_msg = NULL,
						 .will_msg_len = 0,
						 .will_qos = 0,
						 .will_retain = 0
#if LWIP_ALTCP && LWIP_ALTCP_TLS
						 ,
						 NULL
#endif
	};
	void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
				   u8_t flags);
	void mqtt_incoming_publish_cb(void *arg, const char *topic,
				      u32_t tot_len);
	void mqtt_request_cb(void *arg, err_t status);
	void mqtt_connect_cb(mqtt_client_t *client, void *arg,
			     mqtt_connection_status_t status);

    protected:
	mqtt_connection_status_t clientStatus = MQTT_CONNECT_DISCONNECTED;
	bool lwipLink = false;
	bool dhcpLink = false;

    public:
	int clientInit(void);
	void clientConnect(ip_addr_t brokerIP, u16_t brokerPort);
	void clientDisConnect(void);
	void clientSubscribe(const char *postfix, u8_t qos, err_t *status);

	void setClientName(const char *name);
	void setClientID(const char *id);
	void setClientPass(const char *pass);
	virtual ~Network(void);
};
