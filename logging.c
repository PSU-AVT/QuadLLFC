#include "logging.h"

#include <string.h>

void logging_send_string(LOGGING_LEVEL level, const char *str) {
	uint16_t len = strlen(str);
	logging_send_buff(level, str, len);
}

void logging_send_buff(LOGGING_LEVEL level, const unsigned char *buff, uint16_t buff_len) {
}
