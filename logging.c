#include "logging.h"
#include "core/uart.h"
#include "afproto.h"

#include <string.h>

void logging_send_string(LOGGING_LEVEL level, const char *str) {
	uint16_t len = strlen(str);
	logging_send_buff(level, str, len);
}

void logging_send_buff(LOGGING_LEVEL level, const unsigned char *buff, uint16_t buff_len) {
	if(level < LOG_LEVEL)
		return;

	unsigned char *out_buff = uartGetOutputBuffer();
	out_buff[0] = level+1; // Convert to command id
	strncpy(&out_buff[1], buff, buff_len);
	buff_len = afproto_serialize_payload(out_buff, buff_len, out_buff);
	uartSend(out_buff, buff_len);
}
