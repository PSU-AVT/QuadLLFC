#include "state.h"
#include "rotation.h"
#include "sensors/itg3200.h"
#include "core/systick.h"

#define STATE_DOF_CNT 6

static float rotation_b_to_i[3][3]; // Body to inertial rotation matrix
static state_t inertial_state;
static int inertial_needs_update;

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

static uint32_t last_gyro_update_ticks;

void state_update_from_gyro(void) {
	GyroData gd;
	if(itg3200GetData(&gd)) {
		uint32_t tick_diff = systickGetTicks() - last_gyro_update_ticks;
		float dt = tick_diff / 1000;
		rotation_matrix_velocity_update(rotation_b_to_i, gd.X, gd.Y, gd.Z, dt);
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

