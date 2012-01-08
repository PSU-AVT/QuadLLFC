#include "state.h"
#include "rotation.h"
#include "sensors/itg3200.h"
#include "core/systick.h"
#include "commands.h"

#define STATE_GYRO_UPDATE_INTERVAL 5
#define STATE_DOF_CNT 6

static uint32_t _state_send_interval = 20;

static float rotation_b_to_i[3][3]; // Body to inertial rotation matrix
static state_t inertial_state;
static int inertial_needs_update;

static uint32_t _state_gyro_last_update;
static uint32_t _state_send_last;
static GyroData _state_gyro_last;

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
	itg3200Calibrate(&_state_gyro_last, 100, STATE_GYRO_UPDATE_INTERVAL);
}

void state_reset(void) {
	inertial_needs_update = 1;
	state_init();
}

static uint32_t last_gyro_update_ticks;

void state_update_from_gyro(void) {
	if(itg3200GetData(&_state_gyro_last)) {
		uint32_t tick_diff = systickGetTicks() - last_gyro_update_ticks;
		float dt = tick_diff / 1000.0;
		rotation_matrix_velocity_update(rotation_b_to_i, _state_gyro_last.X, _state_gyro_last.Y, _state_gyro_last.Z, dt);
		last_gyro_update_ticks = systickGetTicks();
	}
	inertial_needs_update = 1;
}

void state_inertial_update(void) {
	if(!inertial_needs_update)
		return;
	float atten[3];
	rotation_matrix_get_eulers(rotation_b_to_i, atten);
	inertial_state.roll = atten[0];
	inertial_state.pitch = atten[1];
	inertial_state.yaw = atten[2];
	inertial_needs_update = 0;
}

state_t *state_inertial_get(void) {
	state_inertial_update();
	return &inertial_state;
}

void state_update(void) {
	uint32_t ticks = systickGetTicks();
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
