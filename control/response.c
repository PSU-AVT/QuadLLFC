#include "../sys/tasks.h"
#include "state.h"
#include "movement.h"

volatile static struct task_t _response_task;

void responseUpdate(struct task_t *task)
{
	struct state_controller_t *sc;
	sc = stateControllerGet();

	float roll_torque = 0, pitch_torque = 0, yaw_torque = 0,
	      x, y;

	x = sc->gyros.X + CFG_GYRO_X_BIAS;
	y = sc->gyros.Y + CFG_GYRO_Y_BIAS;

	// P
	if(sc->roll > 2 || sc->roll < -2)
		roll_torque = sc->roll * CFG_PID_P_FACTOR;

	if(sc->pitch > 2 || sc->pitch < -2)
		pitch_torque = sc->pitch * CFG_PID_P_FACTOR;
	yaw_torque = sc->yaw * CFG_PID_P_FACTOR;

	// D
	if(x > 2 || x < -2)
		roll_torque += x * CFG_PID_D_FACTOR;
	if(y > 2 || y < -2)
		pitch_torque += y * CFG_PID_D_FACTOR;
	yaw_torque = sc->gyros.Z * CFG_PID_D_FACTOR;

	movement_roll(roll_torque);
	movement_pitch(pitch_torque);
	//movement_yaw(yaw_torque);

	motorsSyncDutyCycle();
}

void responseStart(void)
{
	_response_task.handler = responseUpdate;
	_response_task.msecs = CFG_RESPONSE_UPDATE_MSECS;

	tasks_add_task(&_response_task);
}
