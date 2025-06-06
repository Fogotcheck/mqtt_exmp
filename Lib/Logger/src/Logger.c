#include "main.h"

#include "Logger.h"

static UART_HandleTypeDef *handle = NULL;

void logger_init(UART_HandleTypeDef *huart)
{
	handle = huart;
}

int _write(int file, char *ptr, int len)
{
	if (handle)
		HAL_UART_Transmit(handle, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
}
