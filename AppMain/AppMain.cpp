#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "cmsis_os2.h"

#include "HW_StmNetwork.h"

void MainThr(__attribute__((unused)) void *arg);

int main(void)
{
	system_init();
	BaseType_t ret = xTaskCreate(MainThr, "MainTask", 500, NULL, 1, NULL);
	if (ret != pdPASS) {
		Error_Handler();
	}
	vTaskStartScheduler();
	Error_Handler();
}

void MainThr(__attribute__((unused)) void *arg)
{
	if (osKernelInitialize()) {
		Error_Handler();
	}
	HW_StmNetwork network;

	if (network.lwipInit())
		Error_Handler();

	if (network.networkInit())
		Error_Handler();

	while (1) {
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		vTaskDelay(1000);
	}
}
