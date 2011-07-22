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
#include "../adc/adc.h"
#include "response.h"
#include "translate.h"

static struct state_controller_t _stateController;

static struct task_t gyro_update_task,
                     state_debug_task;

static float gyro_old_vals[2][3];
static float gyro_int_dt;
static int gyro_int_repetition;

void state_debug(struct task_t *task)
{
	char buff[256];
	struct state_controller_t *sc;
	sc = stateControllerGet();
	sprintf(buff, "State:  \t%f\t%f\t%f\r\ndState/dt\t%f\t%f\t%f\r\n\r\n",
			sc->body_state[Roll], sc->body_state[Pitch], sc->body_state[Yaw],
			sc->body_state_dt[Roll], sc->body_state_dt[Pitch], sc->body_state_dt[Yaw]);
	uartSend(buff, strlen(buff));
}

#define SIMPSONS(S1, S2, S3, dt) ((dt/6.0) * (S1+(4*S2)+S3))

// In this method we obtain our gyro inputs as body_state_dt
// We then integrate using simpsons rule to obtain body_state
// We then rotate this body state to obtain delta_initial_state
// We then add this to our inertial_state to obtain new inertial state
void stateGyroUpdate(struct task_t *task)
{
	char buff[255];

	// update gyro data
	itg3200GetData(&_stateController.gyros);

	// Low pass filter on gyro vals into state_dt
	// state_dt_(n) = state_dt(n-1) * (1 - alpha) + gyro_data * alpha
	_stateController.body_state_dt[Roll] = _stateController. body_state_dt[Roll] * (1 - CFG_GYRO_FILTER_ALPHA) + ((_stateController.gyros.X + CFG_GYRO_X_BIAS) * CFG_GYRO_FILTER_ALPHA);
	_stateController.body_state_dt[Pitch] = _stateController.body_state_dt[Pitch] * (1 - CFG_GYRO_FILTER_ALPHA) + ((_stateController.gyros.Y + CFG_GYRO_Y_BIAS) * CFG_GYRO_FILTER_ALPHA);
	_stateController.body_state_dt[Yaw] = _stateController.body_state_dt[Yaw] * (1 - CFG_GYRO_FILTER_ALPHA_YAW) + ((_stateController.gyros.Z + CFG_GYRO_Z_BIAS) * CFG_GYRO_FILTER_ALPHA_YAW);

	// Integration for attenuation state
	// Uses simpsons rule (requres 3 samples)
	if(gyro_int_repetition == 2) {
		_stateController.body_state[Roll] += SIMPSONS(gyro_old_vals[0][0], gyro_old_vals[1][0], _stateController.body_state_dt[Roll], gyro_int_dt);
		_stateController.body_state[Pitch] += SIMPSONS(gyro_old_vals[0][1], gyro_old_vals[1][1], _stateController.body_state_dt[Pitch], gyro_int_dt);
		_stateController.body_state[Yaw] += SIMPSONS(gyro_old_vals[0][2], gyro_old_vals[1][2], _stateController.body_state_dt[Yaw], gyro_int_dt);

		// Rotate from the Body frame to the Inertial (Earth) frame
		// This loads _stateController.inertial_state
		//translateB2I(&_stateController);

		gyro_int_repetition = 0;
		gyro_int_dt = 0;
	} else {
		// Load state_dt vals into buffer for simpsons rule
		gyro_old_vals[gyro_int_repetition][0] = _stateController.body_state_dt[Roll];
		gyro_old_vals[gyro_int_repetition][1] = _stateController.body_state_dt[Pitch];
		gyro_old_vals[gyro_int_repetition][2] = _stateController.body_state_dt[Yaw];

		gyro_int_repetition++;
		gyro_int_dt += task_get_dt(task);
	}
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
}

void stateStart(void)
{
	gyro_update_task.handler = stateGyroUpdate;
	gyro_update_task.msecs = CFG_GYRO_UPDATE_MSECS;
	state_debug_task.handler = state_debug;
	state_debug_task.msecs = CFG_STATE_OUTPUT_MSECS;

	tasks_add_task(&gyro_update_task);
	//tasks_add_task(&state_debug_task);
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
