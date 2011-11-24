/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "logging.h"
#include "core/uart.h"
#include "afproto.h"

#include <string.h>

void logging_send_string(LOGGING_LEVEL level, const char *str) {
	uint16_t len = 0;
	while(str[++len]);
	logging_send_buff(level, (const unsigned char*)str, len);
}

void logging_send_buff(LOGGING_LEVEL level, const unsigned char *buff, uint16_t buff_len) {
	if(level < LOG_LEVEL)
		return;

	unsigned char *out_buff = uartGetOutputBuffer();
	unsigned char *afproto_buff = afproto_get_buffer();
	out_buff[0] = level+1; // Convert to command id
	int i = 1;
	while(buff_len--) { out_buff[i] = buff[i-1]; i++; }
	buff_len = afproto_serialize_payload(out_buff, i, afproto_buff);
	uartSend(afproto_buff, buff_len);
}
