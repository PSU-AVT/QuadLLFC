#include "response.h"
#include "../sys/tasks.h"
#include "state.h"
#include "../systick/systick.h"
#include "movement.h"

static struct response_controller_t _rc;

static struct task_t _response_task;

static float last_thrust_y;

#define TCOS(x) (1 - ((x*x) / 2) + ((x*x*x*x) / 24) - ((x*x*x*x*x*x)/720))

void responseUpdate(struct task_t *task)
{
	struct state_controller_t *sc;
	sc = stateControllerGet();
	float dt = task_get_dt(task);
	float y_thrust, y_err;

	// P
	// Stabilize Attenuation
	movement_roll(sc->state[Roll] * CFG_PID_P_ROLL * dt);
	movement_pitch(sc->state[Pitch] * CFG_PID_P_PITCH * dt);
	//movement_yaw(sc->state[Yaw] * CFG_PID_P_YAW);

	// Maintain vertical thrust
	y_thrust = (_rc.state[Y]/TCOS(sc->state[Roll]*0.0174532925)) + (_rc.state[Y]/TCOS(sc->state[Pitch]*0.0174532925));
	y_err = y_thrust - last_thrust_y;
	movement_y(y_err);
	last_thrust_y = y_thrust;

	// D
	movement_roll(sc->state_dt[Roll] * CFG_PID_D_FACTOR * dt);
	movement_pitch(sc->state_dt[Pitch] * CFG_PID_D_FACTOR * dt);
	//movement_yaw(sc->gyros.Z * CFG_PID_D_FACTOR);

	motorsSyncDutyCycle();
}

void responseStart(void)
{
	_response_task.handler = responseUpdate;
	_response_task.msecs = CFG_RESPONSE_UPDATE_MSECS;
	_rc.state[Y] = 4;

	tasks_add_task(&_response_task);
}

struct response_controller_t *responseControllerGet(void)
{
	return &_rc;
}
