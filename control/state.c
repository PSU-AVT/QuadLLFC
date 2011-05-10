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

void stateRollBiasCorrect(struct task_t *task);
void statePitchBiasCorrect(struct task_t *task);
void stateRollUpdate(struct task_t *task);
void statePitchUpdate(struct task_t *task);

static struct state_controller_t _stateController;

static struct task_t state_angular_update_task;

void state_angular_update(struct task_t *task)
{
}

struct state_controller_t *stateControllerGet(void)
{
	return &_stateController;
}

void stateInit(void)
{
	// Initialize gyros
	gyro3dInit(&_stateController.gyros, CFG_ROLL_ADC_PIN, CFG_PITCH_ADC_PIN, CFG_YAW_ADC_PIN);
}

void stateStart(void)
{
	_stateController.gyros.roll.base_val = CFG_ROLL_BASE_VAL;
	_stateController.gyros.roll.val = CFG_ROLL_BASE_VAL;

	_stateController.gyros.pitch.base_val = CFG_PITCH_BASE_VAL;
	_stateController.gyros.pitch.val = CFG_PITCH_BASE_VAL;

	_stateController.gyros.yaw.base_val = CFG_YAW_BASE_VAL;
	_stateController.gyros.yaw.val = CFG_YAW_BASE_VAL;

	_stateController.roll.angle = 0;
	_stateController.pitch.angle = 0;
	_stateController.yaw.angle = 0;

	// Start gyros
	gyro3dStart(&_stateController.gyros, CFG_ROLL_GYRO_FILTER_MSECS, CFG_PITCH_GYRO_FILTER_MSECS, CFG_YAW_GYRO_FILTER_MSECS);

	// Create angular state update task
	state_angular_update_task.handler = state_angular_update;
	state_angular_update_task.msecs = CFG_ANGULAR_STATE_UPDATE_MSECS;
	tasks_add_task(&state_angular_update_task);
}
