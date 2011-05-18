/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Gregory Haynes, Spencer Krum
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

static struct task_t gyro_update_task,
                     atenn_update_task,
                     state_debug_task;

void state_debug(struct task_t *task)
{
	char buff[256];
	struct state_controller_t *sc;
	sc = stateControllerGet();
	sprintf(buff, "%f\t%f\t%f\t%f\t%f\t%f\r\n", sc->minus_0.roll_vel, sc->minus_0.pitch_vel, sc->minus_0.yaw_vel,
			sc->roll, sc->pitch, sc->yaw);
	uartSend(buff, strlen(buff));
}

void stateGyroUpdate(struct task_t *task)
{
    //move everything back one unit in time
	copy_struct(&_stateController.minus_2, &_stateController.minus_1);
	copy_struct(&_stateController.minus_1, &_stateController.minus_0);
	//_stateController.minus0 is dealt with below

	itg3200GetData(&_stateController.gyros);
	
	//filtering with  configurable variables
	_stateController.minus_0.roll_vel = _stateController.minus_0.roll_vel * (1 - CFG_GYRO_FILTER_ALPHA) + ((_stateController.gyros.X + CFG_GYRO_X_BIAS) * CFG_GYRO_FILTER_ALPHA);
	_stateController.minus_0.pitch_vel = _stateController.minus_0.pitch_vel * (1 - CFG_GYRO_FILTER_ALPHA) + ((_stateController.gyros.Y + CFG_GYRO_Y_BIAS) * CFG_GYRO_FILTER_ALPHA);
	_stateController.minus_0.yaw_vel = _stateController.minus_0.yaw_vel * (1 - CFG_GYRO_FILTER_ALPHA) + (_stateController.gyros.Z * CFG_GYRO_FILTER_ALPHA);


}

void stateAtennUpdate(struct task_t *task)
{
	_stateController.dt_minus2 = _stateController.dt_minus1;
	_stateController.dt_minus1 = _stateController.dt_minus0;
    _stateController.dt_minus0 = task_get_dt(task);

    //calculating our pitch,yaw,roll with simpsons rule numerical
    //approximation of an intergral


    _stateController.pitch = ((_stateController.dt_minus2 + _stateController.dt_minus1 + _stateController.dt_minus0) /6.0) *(_stateController.minus_2.pitch_vel + 4 * _stateController.minus_1.pitch_vel + _stateController.minus_0.pitch_vel);
    _stateController.yaw = ((_stateController.dt_minus2 + _stateController.dt_minus1 + _stateController.dt_minus0) /6.0) *(_stateController.minus_2.yaw_vel + 4 * _stateController.minus_1.yaw_vel + _stateController.minus_0.yaw_vel);
    _stateController.roll = ((_stateController.dt_minus2 + _stateController.dt_minus1 + _stateController.dt_minus0) /6.0) *(_stateController.minus_2.roll_vel + 4 * _stateController.minus_1.roll_vel + _stateController.minus_0.roll_vel);

}


struct state_controller_t *stateControllerGet(void)
{
	return &_stateController;
}

void copy_struct(struct roll_pitch_yaw_vel * foo, struct roll_pitch_yaw_vel * bar){
	foo->pitch_vel = bar->pitch_vel;
	foo->yaw_vel = bar->yaw_vel;
	foo->roll_vel = bar->roll_vel;
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
	atenn_update_task.handler = stateAtennUpdate;
	atenn_update_task.msecs = CFG_ATENN_UPDATE_MSECS;
	state_debug_task.handler = state_debug;
	state_debug_task.msecs = CFG_STATE_OUTPUT_MSECS;

	tasks_add_task(&gyro_update_task);
	tasks_add_task(&atenn_update_task);
	tasks_add_task(&state_debug_task);
}
