#pragma once

#include <map>
#include <functional>

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

class CoreServiceHW {
    private:
	const StackType_t stackSize = (configMINIMAL_STACK_SIZE << 3);
	const UBaseType_t priority = osPriorityNormal;
	const char *xNetTaskName = "CoreServiceHW";
	const std::function<void(uint32_t)> eventHandle;

	EventGroupHandle_t xEventGroup = nullptr;
	TaskHandle_t taskHandle = nullptr;

    public:
	enum SERVICE_CORE_HW_PARAM { CORE_HW_PARAM_TASK_HANDLE = 0 };
	int eventsThr(void);
	void getParam(uint16_t nParam, void **param);
	uint32_t callEvents(uint32_t event);
	void init(void);
	CoreServiceHW(std::function<void(uint32_t)> handle);
	~CoreServiceHW(void);
};
