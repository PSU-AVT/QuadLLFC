/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "setpoint.h"
#include "commands.h"

static state_t _setpoint;

state_t *setpoint_get(void) {
	return &_setpoint;
}

void setpoint_send(void) {
	command_send(COMMAND_SETPOINT, (unsigned char*)&_setpoint, sizeof(state_t));
}

