#include "setpoint.h"

static state_t _setpoint;

state_t *setpoint_get(void) {
	return &_setpoint;
}

