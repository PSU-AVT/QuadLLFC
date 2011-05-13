#include "../sys/tasks.h"
#include "state.h"
#include "motor.h"

static struct task_t _response_task;

static float roll_prev;
static float pitch_prev;

void responseUpdate(struct task_t *task)
{
	struct state_controller_t *sc;
	sc = stateControllerGet();



	// D
	motorNdxThrustIncrease(MOTOR_LF, PID_D_TERM*sc->gyros.Y);
	motorNdxThrustIncrease(MOTOR_RR, PID_D_TERM*sc->gyros.Y);
	motorNdxThrustIncrease(MOTOR_LR, PID_D_TERM*sc->gyros.X);
	motorNdxThrustIncrease(MOTOR_RF, PID_D_TERM*sc->gyros.X);

	motorsSyncDutyCycle();

}

void responseStart(void)
{
	_response_task.handler = responseUpdate;
	_response_task.msecs = CFG_RESPONSE_UPDATE_MSECS;

	tasks_add_task(&_response_task);

	struct state_controller_t *sc;
	sc = stateControllerGet();
	roll_prev = sc->roll.angle;
	pitch_prev = sc->pitch.angle;
}
