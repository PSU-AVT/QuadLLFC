#include "control.h"
#include "setpoint.h"
#include "core/systick.h"
#include "state.h"
#include "esc.h"

#define CONTROL_UPDATE_INTERVAL 20

static state_t _control_p_gains[4];
static state_t _control_i_gains[4];
static state_t _control_d_gains[4];

static uint32_t _control_last_update;
static state_t _control_setpoint_error_last;
static state_t _control_setpoint_error_integral;

static uint32_t _control_enabled;

void control_init(void) {
	// TODO
	// Set the gain values
        int i, j;
        
	_control_p_gains[0].roll = .01;
	_control_p_gains[2].roll = -.01;
}

void control_reset(void) {
	state_scale(&_control_setpoint_error_last, 0, &_control_setpoint_error_last);
	state_scale(&_control_setpoint_error_integral, 0,
	            &_control_setpoint_error_integral);
}

void control_set_enabled(int value) {
	_control_enabled = value;
}

int control_enabled(void) {
	return _control_enabled;
}

void control_state_gains_multiply_to_motors(state_t *gains,
                                            state_t *error,
                                            float *motors) {
	int i;
	for(i = 0;i < ESC_CNT;i++) {
                motors[i] += gains->roll * error->roll;
                motors[i] += gains->pitch * error->pitch;
                motors[i] += gains->yaw * error->yaw;
                motors[i] += gains->z * error->z;
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
	_control_last_update = systickGetTicks();

	// Calculate p error
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

        /*This is a test, putting in the code from v1
	// Accumulate P
	control_state_gains_multiply_to_motors(&_control_p_gains,
	                                       &setpoint_error,
	                                       motor_accum);

	// Accumulate I
	control_state_gains_multiply_to_motors(&_control_i_gains,
	                                       &setpoint_error,
	                                       motor_accum);

	// Accumulate D
	control_state_gains_multiply_to_motors(&_control_d_gains,
	                                       &setpoint_error,
	                                       motor_accum);
        */
        int j;
        //for(i = 0;i < 3;i++) {
        //p
        for(j = 0;j < 4;j++) {
                motor_accum[j] += setpoint_error.roll*_control_p_gains[j].roll + setpoint_error.pitch*_control_p_gains[j].pitch +
                        setpoint_error.yaw*_control_p_gains[j].yaw + setpoint_error.z*_control_p_gains[j].z;
        }
        //d
        for(j = 0;j < 4;j++) {
                motor_accum[j] += error_dt.roll*_control_d_gains[j].roll + error_dt.pitch*_control_d_gains[j].pitch +
                        error_dt.yaw*_control_d_gains[j].yaw + error_dt.z*_control_d_gains[j].z;
        }
        //i
        for(j = 0;j < 4;j++) {
                motor_accum[j] += error_integral_slice.roll*_control_i_gains[j].roll + error_integral_slice.pitch*_control_i_gains[j].pitch +
                        error_integral_slice.yaw*_control_i_gains[j].yaw + error_integral_slice.z*_control_i_gains[j].z;
        }
                //}
	esc_set_all_throttles(motor_accum);
}

void control_set_p_gains(const state_t *gains) {
        int i;
        for (i =0; i <4; i++)
        {
                state_copy(&gains[i], &_control_p_gains[i]);
        }
}

void control_set_i_gains(const state_t *gains) {
        int i;
        for (i =0; i <4; i++)
        {
                state_copy(&gains[i], &_control_i_gains[i]);
        }
}

void control_set_d_gains(const state_t *gains) {
        int i;
        for (i =0; i <4; i++)
        {
                state_copy(&gains[i], &_control_d_gains[i]);
        }
}
