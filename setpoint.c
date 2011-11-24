/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "setpoint.h"

static state_t _setpoint;

state_t *setpoint_get(void) {
	return &_setpoint;
}

