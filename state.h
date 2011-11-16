#ifndef STATE_H
#define STATE_H

typedef struct state_t {
	float roll;
	float pitch;
	float yaw;
	float x;
	float y;
	float z;
} state_t;

void state_add(state_t *s1, state_t *s2, state_t *sum);
void state_scale(state_t *s1, float val, state_t *result);

state_t *state_get_inertial(void);

#endif 
