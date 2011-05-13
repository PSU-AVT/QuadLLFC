#include "../sys/tasks.h"
#include "state.h"
#include "movement.h"

static struct task_t _response_task;

void responseUpdate(struct task_t *task)
{
	struct state_controller_t *sc;
	sc = stateControllerGet();

	float roll_torque, pitch_torque, yaw_torque;

	// P
	roll_torque = sc->roll * CFG_PID_P_FACTOR;
	pitch_torque = sc->pitch * CFG_PID_P_FACTOR;
	yaw_torque = sc->yaw * CFG_PID_P_FACTOR;

	// D
	roll_torque += sc->gyros.X * CFG_PID_D_FACTOR;
	pitch_torque += sc->gyros.Y * CFG_PID_D_FACTOR;
	yaw_torque += sc->gyros.Z * CFG_PID_D_FACTOR;

	movement_roll(roll_torque);
	movement_pitch(pitch_torque);
	movement_yaw(yaw_torque);

	motorsSyncDutyCycle();
}

void responseStart(void)
{
	_response_task.handler = responseUpdate;
	_response_task.msecs = CFG_RESPONSE_UPDATE_MSECS;

	tasks_add_task(&_response_task);
}
