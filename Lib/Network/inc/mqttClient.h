#pragma once
#include "lwip/apps/mqtt.h"

/**
 * @class mqttClient
 * @brief MQTT client wrapper for lwIP MQTT API.
 */
class mqttClient {
    private:
	mqtt_connection_status_t status; /**< MQTT connection status */
	mqtt_client_t *client =
		nullptr; /**< Pointer to lwIP MQTT client instance */
	mqtt_connect_client_info_t
		ci = { /**< MQTT connection client info structure */
		       .client_id = "NetworkMqtt",
		       .client_user = "NetworkMqtt",
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

    public:
	ip4_addr_t broker = {};
	/**
     * @brief Destructor. Disconnects and frees MQTT client instance.
     */
	~mqttClient();

	/**
     * @brief Initialize MQTT client instance and set incoming publish/data callbacks.
     * @param pub_cb Callback for incoming publish events.
     * @param data_cb Callback for incoming data events.
     * @param arg User argument for callbacks.
     * @return ERR_OK on success, ERR_ARG on error.
     */
	int32_t init(mqtt_incoming_publish_cb_t pub_cb,
		     mqtt_incoming_data_cb_t data_cb, void *arg);

	/**
     * @brief Connect to MQTT broker.
     * @param ipaddr Pointer to broker IP address.
     * @param port Broker port.
     * @param cb Connection callback.
     * @param arg User argument for callback.
     * @return ERR_OK on success, ERR_MEM on error.
     */
	int32_t connect(const ip_addr_t *ipaddr, u16_t port,
			mqtt_connection_cb_t cb, void *arg);

	/**
     * @brief Disconnect from MQTT broker.
     */
	void disconnect();

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
	int32_t publish(const char *topic, const void *payload,
			u16_t payload_length, u8_t qos, u8_t retain,
			mqtt_request_cb_t cb, void *arg);

	/**
     * @brief Subscribe to topic.
     * @param topic Topic string.
     * @param qos Quality of Service.
     * @param cb Subscribe request callback.
     * @param arg User argument for callback.
     * @return ERR_OK on success, ERR_CONN if not connected.
     */
	int32_t subscribe(const char *topic, u8_t qos, mqtt_request_cb_t cb,
			  void *arg);

	/**
     * @brief Set MQTT client ID.
     * @param id Client ID string.
     */
	void setClientId(const char *id);

	/**
     * @brief Set MQTT keep alive interval.
     * @param keep_alive Keep alive value in seconds.
     */
	void setKeepAlive(uint16_t keep_alive);

	/**
     * @brief Set MQTT username.
     * @param user Username string.
     */
	void setUser(const char *user);

	/**
     * @brief Set MQTT password.
     * @param pass Password string.
     */
	void setPass(const char *pass);

	void setStatus(mqtt_connection_status_t st);
};
