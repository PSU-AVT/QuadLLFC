/*
 * Author: Gregory Haynes <greg@greghaynes.net>, K Jonathan Harker <kjharke@cs.pdx.edu>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Gregory Haynes, Spencer Krum, K Jonathan Harker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "state.h"
#include "../config.h"
#include "../control/motor.h"
#include "../systick/systick.h"
#include "../utils/matrix.h"
#include "response.h"
#include "translate.h"

static struct state_controller_t _stateController;

static struct task_t gyro_update_task,
                     state_debug_task;

void state_debug(struct task_t *task)
{
	char buff[512];
	struct state_controller_t *sc;
	sc = stateControllerGet();
	sprintf(buff, "Body State delta: %f\t%f\t%f\r\n"
			      "Body State dt     %f\t%f\t%f\r\n"
			      "Inert State:      %f\t%f\t%f\r\n\r\n",
			sc->body_state_delta[AxisRoll], sc->body_state_delta[AxisPitch], sc->body_state_delta[AxisYaw],
			sc->body_state_dt[AxisRoll], sc->body_state_dt[AxisPitch], sc->body_state_dt[AxisYaw],
			sc->inertial_state[AxisRoll], sc->inertial_state[AxisPitch], sc->inertial_state[AxisYaw]);
	uartSend(buff, strlen(buff));
}

#define SIMPSONS(S1, S2, S3, dt) ((dt/6.0) * (S1+(4*S2)+S3))

// In this method we obtain our gyro inputs as body_state_dt
// We then integrate using simpsons rule to obtain body_state
// We then rotate this body state to obtain delta_initial_state
// We then add this to our inertial_state to obtain new inertial state
void stateGyroUpdate(struct task_t *task)
{
	// update gyro data
	itg3200GetData(&_stateController.gyros);

	// Low pass filter on gyro vals into state_dt
	// state_dt_(n) = state_dt(n-1) * (1 - alpha) + gyro_data * alpha
	_stateController.body_state_dt[AxisRoll] = _stateController. body_state_dt[AxisRoll] * (1 - CFG_GYRO_FILTER_ALPHA) + ((_stateController.gyros.X + CFG_GYRO_X_BIAS) * CFG_GYRO_FILTER_ALPHA);
	_stateController.body_state_dt[AxisPitch] = _stateController.body_state_dt[AxisPitch] * (1 - CFG_GYRO_FILTER_ALPHA) + ((_stateController.gyros.Y + CFG_GYRO_Y_BIAS) * CFG_GYRO_FILTER_ALPHA);
	_stateController.body_state_dt[AxisYaw] = _stateController.body_state_dt[AxisYaw] * (1 - CFG_GYRO_FILTER_ALPHA_YAW) + ((_stateController.gyros.Z + CFG_GYRO_Z_BIAS) * CFG_GYRO_FILTER_ALPHA_YAW);

	float dt = task_get_dt(task);

	_stateController.body_state_delta[AxisRoll] = _stateController.body_state_dt[AxisRoll] * dt;
	_stateController.body_state_delta[AxisPitch] = _stateController.body_state_dt[AxisPitch] * dt;
	_stateController.body_state_delta[AxisYaw] = _stateController.body_state_dt[AxisYaw] * dt;

	// Update the rotation matrix
	rotation_matrix_update(&_stateController);

	// Store eulers as inertial state
	rotation_matrix_get_eulers(_stateController.r_b_to_i, _stateController.inertial_state);
}

#undef SIMPSONS

struct state_controller_t *stateControllerGet(void)
{
	return &_stateController;
}

void stateInit(void)
{
	// Initialize gyros
	itg3200Init();

	int i, j;
	for(i = 0;i < 3;i++) {
		_stateController.body_state_delta[i] = 0;
		_stateController.body_state_dt[i] = 0;
		_stateController.inertial_state[i] = 0;
		for(j = 0;j < 3;j++) {
			if(j == i)
				_stateController.r_b_to_i[i][j] = 1;
			else
				_stateController.r_b_to_i[i][j] = 0;
		}
	}
}

void stateStart(void)
{
	gyro_update_task.handler = stateGyroUpdate;
	gyro_update_task.msecs = CFG_GYRO_UPDATE_MSECS;
	state_debug_task.handler = state_debug;
	state_debug_task.msecs = CFG_STATE_OUTPUT_MSECS;

	tasks_add_task(&gyro_update_task);
	tasks_add_task(&state_debug_task);
}

void stateSubtract(float *a, float *b, float *dest)
{
	int i;
	for(i = 0;i < AXIS_CNT;++i)
		dest[i] = a[i] - b[i];
}

void stateCopy(const float *src, float *dst)
{
	int i;
	for(i = 0;i < AXIS_CNT;++i)
		dst[i] = src[i];
}

void stateAdd(float *a, float *b, float *dest)
{
	int i;
	for(i = 0;i < AXIS_CNT;++i)
		dest[i] = a[i] + b[i];
}
