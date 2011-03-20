/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Gregory Haynes
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

static struct state_controller_t _stateController;

static struct task_t roll_update_task,
                     pitch_update_task,
                     yaw_update_task,
                     roll_gyro_update_task;

struct state_controller_t *stateControllerGet(void)
{
	return &_stateController;
}

void stateInit(void)
{
	gyroInit(&_stateController.roll.gyro, CFG_ROLL_ADC_PIN);
	gyroInit(&_stateController.pitch.gyro, CFG_PITCH_ADC_PIN);
	gyroInit(&_stateController.yaw.gyro, CFG_YAW_ADC_PIN);

	// Gyro update tasks
	roll_gyro_update_task.data = &_stateController.roll.gyro;

	roll_gyro_update_task.handler = gyroUpdateVal;

	roll_gyro_update_task.msecs = CFG_ROLL_GYRO_FILTER_MSECS;

	tasks_add_task(&roll_gyro_update_task);

	// State update tasks
	roll_update_task.data = &_stateController.roll;
	pitch_update_task.data = &_stateController.pitch;
	yaw_update_task.data = &_stateController.yaw;

	roll_update_task.handler = stateAngularUpdate;
	pitch_update_task.handler = stateAngularUpdate;
	yaw_update_task.handler = stateAngularUpdate;

	roll_update_task.msecs = CFG_ROLL_UPDATE_MSECS;
	pitch_update_task.msecs = CFG_PITCH_UPDATE_MSECS;
	yaw_update_task.msecs = CFG_YAW_UPDATE_MSECS;

	tasks_add_task(&roll_update_task);
	tasks_add_task(&pitch_update_task);
	tasks_add_task(&yaw_update_task);
}

void stateStart(void)
{
	gyroStart(&_stateController.roll.gyro);
	gyroStart(&_stateController.pitch.gyro);
	gyroStart(&_stateController.yaw.gyro);

	_stateController.roll.gyro.base_val = 401;
	_stateController.roll.gyro.val = 401;
}

void stateAngularUpdate(struct task_t *task)
{
	struct state_angular_1d_t *state = (struct state_angular_1d_t*)task->data;

	state->angle_vel = gyroGetAngVel(&state->gyro);
	state->angle += state->angle_vel * task_get_dt(task);
}
