#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

typedef enum LOGGING_LEVEL {
	LOGGING_DEBUG = 1,
	LOGGING_ERROR = 2
} LOGGING_LEVEL;

#define LOG_LEVEL LOGGING_DEBUG

void logging_send_string(LOGGING_LEVEL level, const char *str);
void logging_send_buff(LOGGING_LEVEL level, const unsigned char *buff, uint16_t buff_len);

#endif
