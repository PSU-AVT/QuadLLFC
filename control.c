#include "control.h"
#include "setpoint.h"
#include "core/systick.h"
#include "state.h"

#define CONTROL_UPDATE_INTERVAL 20

static state_t _control_p_gains;
static state_t _control_i_gains;
static state_t _control_d_gains;

static uint32_t _control_last_update;
static state_t _control_setpoint_error_last;
static state_t _control_setpoint_error_integral;

static uint32_t _control_enabled;

void control_init(void) {
	// TODO
	// Set the gain values
}

void control_reset(void) {
	state_scale(&_control_setpoint_error_last, 0, &_control_setpoint_error_last);
	state_scale(&_control_setpoint_error_integral, 0,
	            &_control_setpoint_error_integral);
}

void control_set_enabled(int value) {
	_control_enabled = value;
}

void control_state_gains_multiply_to_motors(float *gains,
                                            float *error,
                                            float *motors) {
	int i, j;
	for(i = 0;i < 4;i++) {
		for(j = 0;j < 4;j++) {
			motors[i] += gains[j] * error[j];
		}
	}
}

void control_update(void) {
	// Check if control is enabled
	if(!_control_enabled)
		return;

	// Has enough time passed since last control update
	if((systickGetTicks() - _control_last_update) < 5)
		return;

	state_t setpoint_error;
	state_t error_dt;
	state_t error_integral_slice;

	// Calculate dt and update last_ticks
	uint32_t d_msecs = systickGetTicks() - _control_last_update;
	float dt = d_msecs / 1000.0f;

	// Calculate error
	state_subtract(state_inertial_get(), setpoint_get(), &setpoint_error);

	// Calculate d error / dt
	state_subtract(&setpoint_error, &_control_setpoint_error_last, &error_dt);
	state_scale(&error_dt, 1 / d_msecs, &error_dt);

	// Calculate error integral
	state_scale(&setpoint_error, dt, &error_integral_slice);
	state_add(&error_integral_slice,
	          &_control_setpoint_error_integral,
	          &_control_setpoint_error_integral);

	// Update error_last
	state_copy(&setpoint_error, &_control_setpoint_error_last);

	// Accumulate gains * error
	float motor_accum[4] = { 0, 0, 0, 0 };

	// Accumulate P
	control_state_gains_multiply_to_motors((float*)&_control_p_gains,
	                                       (float*)&setpoint_error,
	                                       motor_accum);

	// Accumulate I
	control_state_gains_multiply_to_motors((float*)&_control_i_gains,
	                                       (float*)&setpoint_error,
	                                       motor_accum);

	// Accumulate D
	control_state_gains_multiply_to_motors((float*)&_control_d_gains,
	                                       (float*)&setpoint_error,
	                                       motor_accum);
}
