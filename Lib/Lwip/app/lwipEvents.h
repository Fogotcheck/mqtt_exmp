#ifndef __LWIP_EVENTS_H__
#define __LWIP_EVENTS_H__

#include "event_groups.h"

#ifdef __cplusplus
extern "C" {
#endif

extern EventGroupHandle_t lwip_event_group;

enum LWIP_EVENT_FLAG {
	LWIP_EVENT_FLAG_LINK_UP = (EventBits_t)(1 << 0),
	LWIP_EVENT_FLAG_LINK_DOWN = (EventBits_t)(1 << 1),
	LWIP_EVENT_FLAG_LINK_DHCP = (EventBits_t)(1 << 2),

	LWIP_EVENTS_ALL = (EventBits_t)(LWIP_EVENT_FLAG_LINK_UP |
					LWIP_EVENT_FLAG_LINK_DOWN |
					LWIP_EVENT_FLAG_LINK_DHCP),
};

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_EVENTS_H__ */
