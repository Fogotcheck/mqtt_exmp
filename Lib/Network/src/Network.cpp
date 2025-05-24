#include "Network.h"

int Network::clientInit(void)
{
	mqtt = mqtt_client_new();
	if (mqtt == nullptr)
		return -1;

	mqtt_set_inpub_callback(
		mqtt,
		[](void *arg, const char *topic, u32_t tot_len) {
			static_cast<Network *>(arg)->mqtt_incoming_publish_cb(
				arg, topic, tot_len);
		},
		[](void *arg, const u8_t *data, u16_t len, u8_t flags) {
			static_cast<Network *>(arg)->mqtt_incoming_data_cb(
				arg, data, len, flags);
		},
		LWIP_CONST_CAST(void *, &mqtt_info));
	return 0;
}

Network::~Network(void)
{
	if (mqtt) {
		clientDisConnect();
		mqtt_client_free(mqtt);
		mqtt = nullptr;
	}
}

void Network::clientConnect(ip_addr_t brokerIP, u16_t brokerPort)
{
	if (mqtt == nullptr) {
		clientStatus = MQTT_CONNECT_DISCONNECTED;
		return;
	}

	mqtt_client_connect(
		mqtt, &brokerIP, brokerPort,
		[](mqtt_client_t *client, void *arg,
		   mqtt_connection_status_t status) {
			static_cast<Network *>(arg)->mqtt_connect_cb(
				client, arg, status);
		},
		LWIP_CONST_CAST(void *, &clientStatus), &mqtt_info);
}

void Network::clientDisConnect(void)
{
	mqtt_disconnect(mqtt);
}

void Network::setClientName(const char *name)
{
	mqtt_info.client_user = name;
}

void Network::setClientID(const char *id)
{
	mqtt_info.client_id = id;
}

void Network::setClientPass(const char *pass)
{
	mqtt_info.client_pass = pass;
}

void Network::clientSubscribe(const char *postfix, u8_t qos, err_t *status)
{
	if (mqtt == nullptr) {
		*status = ERR_MEM;
		return;
	}

	char topic[(MQTT_OUTPUT_RINGBUF_SIZE >> 1)] = {};
	snprintf(topic, sizeof(topic), "%s/%s", mqtt_info.client_user, postfix);
	mqtt_subscribe(
		mqtt, topic, qos,
		[](void *arg, err_t err) {
			static_cast<Network *>(arg)->mqtt_request_cb(arg, err);
		},
		LWIP_CONST_CAST(void *, status));
}

void Network::mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
				    u8_t flags)
{
}
void Network::mqtt_incoming_publish_cb(void *arg, const char *topic,
				       u32_t tot_len)
{
}
void Network::mqtt_request_cb(void *arg, err_t status)
{
	err_t *ret = (err_t *)arg;
	*ret = status;
}
void Network::mqtt_connect_cb(mqtt_client_t *client, void *arg,
			      mqtt_connection_status_t status)
{
	mqtt_connection_status_t *stat = (mqtt_connection_status_t *)arg;
	*stat = status;
}
