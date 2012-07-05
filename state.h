#ifndef STATE_H
#define STATE_H

// euler angles determine these ranges
typedef struct state_t {
	float roll;
	float pitch;
	float yaw;
        // this is in the world coordinate system
	float x; // does not exist
	float y; // does not exist
	float z; // 
} state_t;

void state_add(state_t *s1, state_t *s2, state_t *sum);
/* diff = s1 - s2 */
void state_subtract(state_t *s1, state_t *s2, state_t *diff);
void state_scale(state_t *s1, float val, state_t *result);
void state_copy(const state_t *src, state_t *dest);

void state_init(void);
void state_reset(void);
void state_update_from_gyro(void);
void state_update(void);

state_t *state_inertial_get(void);

void state_set_send_interval(unsigned int msecs);
void state_send(void);

#endif 
