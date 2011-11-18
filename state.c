#include "state.h"
#include "sensors/itg3200.h"

#define STATE_DOF_CNT 6

void state_add(state_t *s1, state_t *s2, state_t *sum) {
	float *s1_arr = (float*)s1;
	float *s2_arr = (float*)s2;
	float *sum_arr = (float*)sum;

	int i;
	for(i = 0;i < STATE_DOF_CNT;++i) sum_arr[i] = s1_arr[i] + s2_arr[i];
}

void state_scale(state_t *s1, float val, state_t *dest) {
	float *s1_arr = (float*)s1;
	float *dest_arr = (float*)dest;

	int i;
	for(i = 0;i < STATE_DOF_CNT;++i) dest_arr[i] = s1_arr[i] * val;
}

void state_update_from_gyro(void) {
	GyroData gd;
	if(itg3200GetData(&gd)) {
		
	}
}

