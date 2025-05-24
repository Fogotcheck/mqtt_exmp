#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "HW_StmNetwork.h"
#include "lwip.h"
#include "lwipEvents.h"

extern struct netif gnetif;

/**
 * @brief  Network event processing task.
 * @param  arg     Pointer to HW_StmNetwork object.
 */
void hw_NetworkThr(void *arg);

/**
 * @brief  Get the handle of the network task.
 * @return TaskHandle_t Network task handle.
 */
TaskHandle_t HW_StmNetwork::getTaskHandle(void)
{
	return taskHandle;
}

/**
 * @brief  Constructor. Initializes Ethernet hardware.
 */
HW_StmNetwork::HW_StmNetwork(void)
{
	MX_ETH_Init();
}

/**
 * @brief  	Destructor. Deinitializes Ethernet hardware and
 * 			deletes the network task.
 */
HW_StmNetwork::~HW_StmNetwork(void)
{
	HAL_ETH_DeInit(&heth);
	if (taskHandle != NULL) {
		vTaskDelete(taskHandle);
	}
}

/**
 * @brief  Initialize the lwIP stack.
 * @return 0 on success, -1 on error.
 */
int HW_StmNetwork::lwipInit(void)
{
	if (MX_LWIP_Init()) {
		return -1;
	}
	return 0;
}

/**
 * @brief  CHeck if lwIP link is up.
 * @return true if lwIP link is up, false otherwise..
 */
bool HW_StmNetwork::isLwipLinkUp(void)
{
	return lwipLink;
}

/**
 * @brief  Check if DHCP link is up.
 * @return true if DHCP link is up, false otherwise.
 */
bool HW_StmNetwork::isDhcpLinkUp(void)
{
	return dhcpLink;
}

/**
 * @brief  Create the network event processing task.
 * @return 0 on success, -1 on error.
 */
int HW_StmNetwork::networkInit(void)
{
	if (pdPASS != xTaskCreate(hw_NetworkThr, xNetTaskName, stackSize, this,
				  priority, &taskHandle))
		return -1;

	if (clientInit())
		return -1;

	return 0;
}

/**
 * @brief  Network event processing task body (static wrapper for FreeRTOS).
 * @param  arg Pointer to HW_StmNetwork object.
 */
void hw_NetworkThr(void *arg)
{
	static_cast<HW_StmNetwork *>(arg)->thrBody();
}

/**
 * @brief  Task body: processes network events in a loop.
 * @return 0 (never returns under normal operation).
 */
int HW_StmNetwork::thrBody(void)
{
	EventBits_t Event = 0;
	EventBits_t Mask = 1;
	while (1) {
		Event = xEventGroupWaitBits(lwip_event_group, LWIP_EVENTS_ALL,
					    pdFALSE, pdFALSE, portMAX_DELAY);
		Mask = 1;
		for (uint8_t i = 0; i < configUSE_16_BIT_TICKS; i++) {
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

/**
 * @brief  Handle a specific network event.
 * @param  Event Event mask.
 * @return 0 on success, -1 on error.
 */
int HW_StmNetwork::eventHandle(EventBits_t Event)
{
	xEventGroupClearBits(lwip_event_group, Event);

	switch (Event) {
	case LWIP_EVENT_FLAG_LINK_DOWN:
		lwipLink = false;
		dhcpLink = false;
		clientStatus = MQTT_CONNECT_DISCONNECTED;
		dhcp_stop(&gnetif);
		return 0;

	case LWIP_EVENT_FLAG_LINK_UP:
		lwipLink = true;
		dhcp_start(&gnetif);
		break;
	case LWIP_EVENT_FLAG_LINK_DHCP:
		dhcpLink = true;
		break;
	default:
		return -1;
	}

	char name[24] = { 0 };
	char id[24] = { 0 };
	ip4_addr_t myIP = gnetif.ip_addr;
	ip4_addr_t brokerIP = {};

	clientDisConnect();
	snprintf(name, sizeof(name), "dev_%u.%u.%u.%u", ip4_addr1(&myIP),
		 ip4_addr2(&myIP), ip4_addr3(&myIP), ip4_addr4(&myIP));
	snprintf(id, sizeof(id), "ip_%u.%u.%u.%u", ip4_addr1(&myIP),
		 ip4_addr2(&myIP), ip4_addr3(&myIP), ip4_addr4(&myIP));

	setClientName(name);
	setClientID(&name[28]);
	/*! \todo add DNS request*/
	ipaddr_aton("192.168.0.1", &brokerIP);
	clientConnect(brokerIP, MQTT_PORT);

	return 0;
}
