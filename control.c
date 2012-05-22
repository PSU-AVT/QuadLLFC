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
static state_t _control_setpoint_error_integral;

static uint32_t _control_enabled;

void control_init(void) {
	// TODO
	// Set the gain values
        
	_control_p_gains[1].roll = -.042;
	_control_p_gains[3].roll = .042;
	_control_d_gains[1].roll = -0;
	_control_d_gains[3].roll = 0;
	_control_i_gains[1].roll = -0;
	_control_i_gains[3].roll = 0;	

	_control_p_gains[0].pitch = .03075;
	_control_p_gains[2].pitch = -.03075;
	_control_d_gains[0].pitch = 0;
	_control_d_gains[2].pitch = -0;
	_control_i_gains[0].pitch = 0;
	_control_i_gains[2].pitch = -0;

	_control_p_gains[0].yaw = -0;
	_control_p_gains[1].yaw = 0;
	_control_p_gains[2].yaw = -0;
	_control_p_gains[3].yaw = 0;
	_control_d_gains[0].yaw = -0;
	_control_d_gains[1].yaw = 0;
	_control_d_gains[2].yaw = -0;
	_control_d_gains[3].yaw = 0;

	_control_p_gains[0].z = 1;
	_control_p_gains[1].z = 1;
	_control_p_gains[2].z = 1;
	_control_p_gains[3].z = 1;
}

void control_reset(void) {
        /* Zero error integral */
	state_scale(&_control_setpoint_error_last, 0,
	            &_control_setpoint_error_last);
	/* Zero error */
	state_scale(&_control_setpoint_error_integral, 0,
	            &_control_setpoint_error_integral);

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

static state_t setpoint_error;
static state_t error_dt;
static state_t error_integral_slice;

void control_update(void) {
	float motor_accum[4] = { 0, 0, 0, 0 };

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

	// Calculate dt and update last_ticks
	uint32_t d_msecs = systickGetTicks() - _control_last_update;
	float dt = d_msecs / 1000.0f;
	_control_last_update = systickGetTicks();

	// Calculate p error
	state_subtract(setpoint_get(), state_inertial_get(), &setpoint_error);

	// Calculate d error / dt
	state_subtract(&setpoint_error, &_control_setpoint_error_last, &error_dt);
	state_scale(&error_dt, dt, &error_dt);

	// Calculate error integral
	state_scale(&setpoint_error, dt, &error_integral_slice);
	state_add(&error_integral_slice,
	          &_control_setpoint_error_integral,
	          &_control_setpoint_error_integral);

	// Update error_last
	state_copy(&setpoint_error, &_control_setpoint_error_last);

	// Accumulate gains * error

        int j;
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
                motor_accum[j] += _control_setpoint_error_integral.roll*_control_i_gains[j].roll + _control_setpoint_error_integral.pitch*_control_i_gains[j].pitch + _control_setpoint_error_integral.yaw*_control_i_gains[j].yaw + _control_setpoint_error_integral.z*_control_i_gains[j].z;
        }
                //}
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

