#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

typedef enum LOGGING_LEVEL {
	LOGGING_DEBUG,
	LOGGING_ERROR
} LOGGING_LEVEL;

void logging_send_string(LOGGING_LEVEL level, const char *str);
void logging_send_buff(LOGGING_LEVEL level, const unsigned char *buff, uint16_t buff_len);

#endif
