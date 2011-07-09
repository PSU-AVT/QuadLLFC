#include "response.h"
#include "../sys/tasks.h"
#include "state.h"
#include "../systick/systick.h"
#include "movement.h"

static struct response_controller_t _rc;
static struct task_t _response_task;

static float state_error_sum[AXIS_CNT] = {0, 0, 0, 0, 0, 0};
static float state_error_last[AXIS_CNT] = {0, 0, 0, 0, 0, 0};

static float state_error_gains[3][AXIS_CNT] = {
		{0, 0, 0, 0, 0, 0}, // P
		{0, 0, 0, 0, 0, 0}, // I
		{0, 0, 0, 0, 0, 0}  // D
};   //  R  P  Y  X  Y  Z

/* This gets called every CFG_RESPONSE_UPDATE_MSECS */
void responseUpdate(struct task_t *task)
{
	int i;
	struct state_controller_t *sc;
	sc = stateControllerGet();

	// Figure out error
	float state_error[AXIS_CNT];
	stateSubtract(_rc.state_setpoint, sc->state, state_error);

	// Update state sum
	stateAdd(state_error, state_error_sum, state_error_sum);

	// P, D
	for(i = 0;i < AXIS_CNT;++i) {
		movement(i, state_error_gains[0][i] * state_error[i]); // P
		movement(i, state_error_gains[1][i] * state_error_sum[i]); // I
		movement(i, state_error_gains[2][i] * (state_error_last[i] - state_error[i])); // D
	}

	stateCopy(state_error, state_error_last);

	motorsSyncDutyCycle();
}

void responseStart(void)
{
	int i = 0;

	// Init _rc
	for(i = 0;i < AXIS_CNT;++i) {
		_rc.state_setpoint[i] = 0;
		_rc.state_dt_setpoint[i] = 0;
	}

	// Setup response update task
	_response_task.handler = responseUpdate;
	_response_task.msecs = CFG_RESPONSE_UPDATE_MSECS;
	tasks_add_task(&_response_task);
}

struct response_controller_t *responseControllerGet(void)
{
	return &_rc;
}
