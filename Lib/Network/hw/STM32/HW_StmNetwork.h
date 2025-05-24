#pragma once

#include "Network.h"

#ifdef __cplusplus
/**
 * @brief Hardware abstraction for STM32 network interface.
 */
class HW_StmNetwork : public Network {
    private:
	TaskHandle_t taskHandle;
	const StackType_t stackSize = (configMINIMAL_STACK_SIZE << 1);
	const UBaseType_t priority = osPriorityRealtime7;
	const char *xNetTaskName = "HW_Network";

    public:
	int thrBody(void);
	int eventHandle(EventBits_t Event);
	TaskHandle_t getTaskHandle(void);

	bool isLwipLinkUp(void);
	bool isDhcpLinkUp(void);

	int lwipInit(void);
	int networkInit(void);

	HW_StmNetwork(void);
	~HW_StmNetwork(void);
};

#endif
