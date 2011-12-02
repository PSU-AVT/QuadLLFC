/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

typedef enum LOGGING_LEVEL {
	LOGGING_DEBUG = 1,
	LOGGING_ERROR = 2,
	LOGGING_NONE = 3
} LOGGING_LEVEL;

void logging_send_string(LOGGING_LEVEL level, const char *str);
void logging_send_buff(LOGGING_LEVEL level, const unsigned char *buff, uint16_t buff_len);
void logging_set_level(LOGGING_LEVEL level);

#endif
