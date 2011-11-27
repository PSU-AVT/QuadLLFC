/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "logging.h"
#include "commands.h"

#include <string.h>

void logging_send_string(LOGGING_LEVEL level, const char *str) {
	uint16_t len = 0;
	while(str[++len]);
	logging_send_buff(level, (const unsigned char*)str, len);
}

void logging_send_buff(LOGGING_LEVEL level, const unsigned char *buff, uint16_t buff_len) {
	if(level < LOG_LEVEL)
		return;

	uint8_t command_id;
	switch(level) {
	case LOGGING_DEBUG:
		command_id = COMMAND_DEBUG;
		break;
	default:
		command_id = COMMAND_ERROR;
	}

	command_send(command_id, buff, buff_len);
}
