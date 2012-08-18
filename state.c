#include "state.h"
#include "rotation.h"
#include "sensors/itg3200.h"
#include "sensors/adxl345.h"
#include "sensors/sharp.h"
#include "sensors/maxbotixs.h"
#include "core/systick.h"
#include "commands.h"
#include "logging.h"

#define STATE_GYRO_UPDATE_INTERVAL 5     //200hz
#define STATE_HEIGHT_UPDATE_INTERVAL 50  //20hz
#define STATE_DOF_CNT 6

static uint32_t _state_send_interval = 500;

static float rotation_b_to_i[3][3]; // Body to inertial rotation matrix
static state_t _inertial_state;
static int inertial_needs_update;

static uint32_t _state_gyro_last_update;
static uint32_t _last_gyro_update_ticks;
static uint32_t _state_send_last;
static GyroData _state_gyro_last;

static uint32_t _state_height_last_update;
static uint8_t sensor; //0 = ir, 1 = sonar, 2 = future! (baro?)

void state_add(state_t *s1, state_t *s2, state_t *sum) {
	float *s1_arr = (float*)s1;
	float *s2_arr = (float*)s2;
	float *sum_arr = (float*)sum;

	int i;
	for(i = 0;i < STATE_DOF_CNT;++i) sum_arr[i] = s1_arr[i] + s2_arr[i];
}

void state_subtract(state_t *s1, state_t *s2, state_t *diff) {
	float *s1_arr = (float*)s1;
	float *s2_arr = (float*)s2;
	float *diff_arr = (float*)diff;

	int i;
	for(i = 0;i < STATE_DOF_CNT;++i) diff_arr[i] = s1_arr[i] - s2_arr[i];
}

void state_scale(state_t *s1, float val, state_t *dest) {
	float *s1_arr = (float*)s1;
	float *dest_arr = (float*)dest;

	int i;
	for(i = 0;i < STATE_DOF_CNT;++i) dest_arr[i] = s1_arr[i] * val;
}

void state_copy(const state_t *src, state_t *dest) {
	float *src_arr = (float*)src;
	float *dest_arr = (float*)dest;

	int i;
	for(i = 0;i < STATE_DOF_CNT;++i) dest_arr[i] = src_arr[i];
}

void state_init(void) {
	rotation_matrix_init(rotation_b_to_i);
	logging_send_string(LOGGING_DEBUG, "Calibrating gyro.");
	itg3200Calibrate(&_state_gyro_last, 1000, STATE_GYRO_UPDATE_INTERVAL);
	logging_send_string(LOGGING_DEBUG, "Calibrating gyro complete.");
        SharpInit(ADC_PIN5);
        init_maxbotix();
}

void state_reset(void) {
	inertial_needs_update = 1;
	state_init();
}

void state_update_height() {
        if (_inertial_state.z < 20)
                sensor = 0;
        else if (_inertial_state.z > 25)
                sensor = 1;

        switch (sensor)
        {
        case 0:
                _inertial_state.z = Sharp120XGetDistance(ADC_PIN5);
                break;
        case 1:
                _inertial_state.z = measure_maxbotix_cm();
                break;
        default:
                break;
        }
}

void state_update_from_gyro(void) {
	if(itg3200GetData(&_state_gyro_last) == i2c_ok) {
		uint32_t tick_diff = systickGetTicks() - _last_gyro_update_ticks;
	
		// Set dt in seconds
		float dt;
		if(_last_gyro_update_ticks == 0)
			dt = 0;
		else
			dt = tick_diff / 1000.0;

		// Update rotation matrix
		rotation_matrix_velocity_update(rotation_b_to_i, _state_gyro_last.X, _state_gyro_last.Y, _state_gyro_last.Z, dt);

		// Update last update ticks
		_last_gyro_update_ticks = systickGetTicks();
	} else 
		command_send(COMMAND_ERROR, (unsigned char*)"Invalid gyro read.", 18);

	inertial_needs_update = 1;
}

void state_inertial_update(void) {
	if(!inertial_needs_update)
		return;
	float atten[3];
	rotation_matrix_get_eulers(rotation_b_to_i, atten);
	_inertial_state.roll = atten[0];
	_inertial_state.pitch = atten[1];
	_inertial_state.yaw = atten[2];
	inertial_needs_update = 0;
}

state_t *state_inertial_get(void) {
	state_inertial_update();
	return &_inertial_state;
}

void state_update(void) {
	uint32_t ticks = systickGetTicks();

        if((ticks - _state_height_last_update) >= STATE_HEIGHT_UPDATE_INTERVAL){
                state_update_height();
                _state_height_last_update = ticks;
        }
        ticks = systickGetTicks();
	if((ticks - _state_gyro_last_update) >= STATE_GYRO_UPDATE_INTERVAL) {
		state_update_from_gyro();
		_state_gyro_last_update = ticks;
	}
	ticks = systickGetTicks();
	if(_state_send_interval && (ticks - _state_send_last) >= _state_send_interval) {
		state_send();
		_state_send_last = ticks;
	}
}

void state_set_send_interval(unsigned int msecs) {
	_state_send_interval = msecs;
}

void state_send(void) {
	state_t *inertial = state_inertial_get();
	command_send(COMMAND_INERTIAL_STATE, (unsigned char*)inertial, sizeof(state_t));
}
