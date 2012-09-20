#include "control.h"
#include "setpoint.h"
#include "core/systick.h"
#include "state.h"
#include "esc.h"
#include "logging.h"

#define CONTROL_UPDATE_INTERVAL 20

static state_t _control_p_gains[4];
static state_t _control_i_gains[4];
static state_t _control_d_gains[4];

static uint32_t _control_last_update;
static state_t _control_setpoint_error_last;

static uint32_t _control_enabled;

static float _control_integral_slice_max[4];
static float _control_integral_max[4];
static float _control_motor_integrals[4];

void control_init(void) {
	// TODO
	// Set the gain values
	//
	_control_p_gains[1].roll = -.065;
	_control_p_gains[3].roll = .065;
	_control_d_gains[1].roll = -440.0;
	_control_d_gains[3].roll = 440.0;
	_control_i_gains[1].roll = -.03;	
	_control_i_gains[3].roll = .03;	

	_control_p_gains[0].pitch = .065;
	_control_p_gains[2].pitch = -.065;
	_control_d_gains[0].pitch = 440.0;
	_control_d_gains[2].pitch = -440.0;
	_control_i_gains[0].roll = -.03;	
	_control_i_gains[2].roll = .03;	

	_control_p_gains[0].yaw = -0.075;
	_control_p_gains[1].yaw = 0.075;
	_control_p_gains[2].yaw = -0.075;
	_control_p_gains[3].yaw = 0.075;
	_control_d_gains[0].yaw = -240.0;
	_control_d_gains[1].yaw = 240.0;
	_control_d_gains[2].yaw = -240.0;
	_control_d_gains[3].yaw = 240.0;

	_control_p_gains[0].z = 1;
	_control_p_gains[1].z = 1;
	_control_p_gains[2].z = 1;
	_control_p_gains[3].z = 1;

	_control_integral_slice_max[0] = _control_p_gains[0].pitch * .05;
	_control_integral_slice_max[1] = _control_p_gains[1].roll * .05;
	_control_integral_slice_max[2] = _control_p_gains[2].pitch * .05;
	_control_integral_slice_max[3] = _control_p_gains[3].roll * .05;

	_control_integral_max[0] = _control_p_gains[0].pitch * .5;
	_control_integral_max[1] = _control_p_gains[1].roll * .5;
	_control_integral_max[2] = _control_p_gains[2].pitch * .5;
	_control_integral_max[3] = _control_p_gains[3].roll * .5;
}

void control_reset(void) {
        /* Zero error integral */
	state_scale(&_control_setpoint_error_last, 0,
	            &_control_setpoint_error_last);

	int i = 0;
	for(i = 0;i < 4;i++) {
		_control_motor_integrals[i] = 0;
	}

        _control_last_update = 0;
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
	int i;
        int j;
	float motor_accum[4] = { 0, 0, 0, 0 };
	state_t setpoint_error;
	state_t error_dt;
	state_t error_integral_slice;

	for(i = 0;i < 6;i++) {
		((float*)&setpoint_error)[i] = 0;
		((float*)&error_dt)[i] = 0;
		((float*)&error_integral_slice)[i] = 0;
	}

	// Check if control is enabled
	if(!_control_enabled)
		return;

	// Has enough time passed since last control update
	if((systickGetTicks() - _control_last_update) < 5)
		return;

	// Safety Third!
	state_t *inertial_state = state_inertial_get();
	if(inertial_state->roll >= 1 || inertial_state->roll <= -1 || inertial_state->pitch >= 1 || inertial_state->pitch <= -1) {
		logging_send_string(LOGGING_ERROR, "Shutting down due to extreme attenuation");
		esc_set_all_throttles(motor_accum);	
		_control_enabled = 0;
		return;
	}

	uint32_t d_msecs = systickGetTicks() - _control_last_update;
	float dt = d_msecs / 1000.0f;

	// Calculate p error
	state_subtract(setpoint_get(), state_inertial_get(), &setpoint_error);

	float motor_slice[4];
	if(_control_last_update != 0) {
		// Calculate error integral
		state_scale(&setpoint_error, dt, &error_integral_slice);

		// Calculate d error / dt
		state_subtract(&setpoint_error, &_control_setpoint_error_last, &error_dt);
		state_scale(&error_dt, dt, &error_dt);

		for(i = 0;i < 4;i++) {
			float *motor_i_gains = (float*)(&_control_i_gains[i]);
			motor_slice[i] = 0;
			for(j = 0;j < 6;j++) {
				motor_slice[i] += ((float*)&error_integral_slice)[j] * motor_i_gains[j];
			}
	/*
			if(motor_slice[i] > _control_integral_slice_max[i])
				motor_slice[i] = _control_integral_slice_max[i];
			else if (motor_slice[i] < -_control_integral_slice_max[i])
				motor_slice[i] = -_control_integral_slice_max[i];
	*/
			_control_motor_integrals[i] += motor_slice[i];
	/*

			// Check for max motor integral val
			if(_control_motor_integrals[i] > _control_integral_max[i])
				_control_motor_integrals[i] = _control_integral_max[i];
			else if(_control_motor_integrals[i] < -_control_integral_max[i])
				_control_motor_integrals[i] = -_control_integral_max[i];
	*/
		}
	}
	_control_last_update = systickGetTicks();

	// Update error_last
	state_copy(&setpoint_error, &_control_setpoint_error_last);

	if(_control_last_update == 0)
		return;

	// Accumulate gains * error
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
        for(j = 0;j < 4;j++)
		motor_accum[j] += _control_motor_integrals[j];

	// Apply throttles
	esc_set_all_throttles(motor_accum);
}

state_t *control_get_p_gains(void) {
	return _control_p_gains;
}

state_t *control_get_i_gains(void) {
	return _control_i_gains;
}

state_t *control_get_d_gains(void) {
	return _control_d_gains;
}

