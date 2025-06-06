#pragma once

#ifdef __cplusplus
extern "C" {
#endif
void logger_init(UART_HandleTypeDef *huart);

#if (DEBUG_LOG > 0)
#define FERROR(...)                                                   \
	{                                                             \
		fprintf(stderr, "%s:%d ERROR: ", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__);                         \
		fprintf(stderr, "\n");                                \
	}
#else
#define FERROR(...)
#endif

#if (DEBUG_LOG > 1)
#define FWARNING(...)                                                   \
	{                                                               \
		fprintf(stderr, "%s:%d WARNING: ", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__);                           \
		fprintf(stderr, "\n");                                  \
	}
#else
#define FWARNING(...)
#endif

#if (DEBUG_LOG > 2)
#define FINFO(...)                                                   \
	{                                                            \
		fprintf(stderr, "%s:%d INFO: ", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__);                        \
		fprintf(stderr, "\n");                               \
	}
#else
#define FINFO(...)
#endif
#ifdef __cplusplus
}
#endif
