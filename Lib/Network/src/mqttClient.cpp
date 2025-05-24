#include "mqttClient.h"

/**
 * @brief Initialize MQTT client instance and set incoming publish/data callbacks.
 * @param pub_cb Callback for incoming publish events.
 * @param data_cb Callback for incoming data events.
 * @param arg User argument for callbacks.
 * @return ERR_OK on success, ERR_ARG on error.
 */
int32_t mqttClient::init(mqtt_incoming_publish_cb_t pub_cb,
			 mqtt_incoming_data_cb_t data_cb, void *arg)
{
	int32_t ret = ERR_ARG;
	client = mqtt_client_new();
	if (client == nullptr) {
		return ret;
	}

	mqtt_set_inpub_callback(client, pub_cb, data_cb, arg);

	ret = ERR_OK;
	return ret;
}

/**
 * @brief Destructor. Disconnects and frees MQTT client instance.
 */
mqttClient::~mqttClient()
{
	disconnect();
	if (client) {
		mqtt_client_free(client);
		client = nullptr;
	}
}

/**
 * @brief Connect to MQTT broker.
 * @param ipaddr Pointer to broker IP address.
 * @param port Broker port.
 * @param cb Connection callback.
 * @param arg User argument for callback.
 * @return ERR_OK on success, ERR_MEM on error.
 */
int32_t mqttClient::connect(const ip_addr_t *ipaddr, u16_t port,
			    mqtt_connection_cb_t cb, void *arg)
{
	int32_t ret = ERR_MEM;
	if (!client)
		return ret;

	ret = mqtt_client_connect(client, ipaddr, port, cb, arg, &ci);
	return ret;
}

/**
 * @brief Disconnect from MQTT broker.
 */
void mqttClient::disconnect()
{
	mqtt_disconnect(client);
	status = MQTT_CONNECT_DISCONNECTED;
}

/**
 * @brief Publish message to topic.
 * @param topic Topic string.
 * @param payload Message payload.
 * @param payload_length Length of payload.
 * @param qos Quality of Service.
 * @param retain Retain flag.
 * @param cb Publish request callback.
 * @param arg User argument for callback.
 * @return ERR_OK on success, ERR_CONN if not connected.
 */
int32_t mqttClient::publish(const char *topic, const void *payload,
			    u16_t payload_length, u8_t qos, u8_t retain,
			    mqtt_request_cb_t cb, void *arg)
{
	int32_t ret = ERR_CONN;
	if ((client == nullptr) || (status != MQTT_CONNECT_ACCEPTED))
		return ret;

	ret = mqtt_publish(client, topic, payload, payload_length, qos, retain,
			   cb, arg);
	return ret;
}

/**
 * @brief Subscribe to topic.
 * @param topic Topic string.
 * @param qos Quality of Service.
 * @param cb Subscribe request callback.
 * @param arg User argument for callback.
 * @return ERR_OK on success, ERR_CONN if not connected.
 */
int32_t mqttClient::subscribe(const char *topic, u8_t qos, mqtt_request_cb_t cb,
			      void *arg)
{
	int32_t ret = ERR_CONN;
	if ((client == nullptr) || (status != MQTT_CONNECT_ACCEPTED))
		return ret;
	ret = mqtt_subscribe(client, topic, qos, cb, arg);
	return ret;
}

/**
 * @brief Set MQTT client ID.
 * @param id Client ID string.
 */
void mqttClient::setClientId(const char *id)
{
	ci.client_id = id;
}

/**
 * @brief Set MQTT keep alive interval.
 * @param keep_alive Keep alive value in seconds.
 */
void mqttClient::setKeepAlive(uint16_t keep_alive)
{
	ci.keep_alive = keep_alive;
}

/**
 * @brief Set MQTT username.
 * @param user Username string.
 */
void mqttClient::setUser(const char *user)
{
	ci.client_user = user;
}

/**
 * @brief Set MQTT password.
 * @param pass Password string.
 */
void mqttClient::setPass(const char *pass)
{
	ci.client_pass = pass;
}

void mqttClient::setStatus(mqtt_connection_status_t st)
{
	status = st;
}
