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

	float roll_d = sc->roll.angle - roll_prev;
	float pitch_d = sc->pitch.angle - pitch_prev;

	// P
	motorNdxThrustIncrease(MOTOR_LF, 50*sc->roll.angle);
	motorNdxThrustIncrease(MOTOR_RR, -50*sc->roll.angle);
	motorNdxThrustIncrease(MOTOR_LR, 50*sc->pitch.angle);
	motorNdxThrustIncrease(MOTOR_RF, -50*sc->pitch.angle);

	// D
	motorNdxThrustIncrease(MOTOR_LF, 4000*roll_d);
	motorNdxThrustIncrease(MOTOR_RR, -4000*roll_d);
	motorNdxThrustIncrease(MOTOR_LR, 4000*pitch_d);
	motorNdxThrustIncrease(MOTOR_RF, -4000*pitch_d);

	motorsSyncDutyCycle();

	roll_prev = sc->roll.angle;
	pitch_prev = sc->pitch.angle;
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
