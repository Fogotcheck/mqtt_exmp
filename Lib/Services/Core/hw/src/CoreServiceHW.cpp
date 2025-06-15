#include <stdexcept>

#include "CoreServiceHW.h"

CoreServiceHW::CoreServiceHW(std::function<void(uint32_t)> handle)
	: eventHandle(handle)
{
}

CoreServiceHW::~CoreServiceHW(void)
{
	vEventGroupDelete(xEventGroup);
	vTaskDelete(taskHandle);
}

void CoreServiceHWThr(void *arg)
{
	static_cast<CoreServiceHW *>(arg)->eventsThr();
}

void CoreServiceHW::init(void)
{
	if (NULL == (xEventGroup = xEventGroupCreate()))
		throw std::runtime_error("Failed to initialize xEventGroup");

	if (pdPASS != xTaskCreate(CoreServiceHWThr, xNetTaskName, stackSize,
				  this, priority, &taskHandle))
		throw std::runtime_error("Failed to initialize xTaskCreate");
}

int CoreServiceHW::eventsThr(void)
{
	EventBits_t Event = 0;
	EventBits_t Mask = 1;
	while (1) {
		Event = xEventGroupWaitBits(this->xEventGroup, 0xFFFFFF,
					    pdFALSE, pdFALSE, portMAX_DELAY);
		Mask = 1;
		for (uint8_t i = 0; i < 24; i++) {
			if (Event & Mask) {
				xEventGroupClearBits(this->xEventGroup,
						     Event & Mask);

				std::invoke(eventHandle, Event & Mask);
			}
			Mask <<= 1;
		}
	}
	return 0;
}

uint32_t CoreServiceHW::callEvents(uint32_t event)
{
	if (this->xEventGroup)
		return osEventFlagsSet(this->xEventGroup, event);
	return 0xFFFFFF;
}

void CoreServiceHW::getParam(uint16_t nParam, void **param)
{
	switch (nParam) {
	case CORE_HW_PARAM_TASK_HANDLE:
		*param = taskHandle;
		break;

	default:
		*param = nullptr;
	}
}
