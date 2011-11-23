#include "control.h"
#include "setpoint.h"
#include "state.h"

void control_update(void) {
	state_t setpoint_error;

	state_subtract(state_inertial_get(), setpoint_get(), &setpoint_error);
}
