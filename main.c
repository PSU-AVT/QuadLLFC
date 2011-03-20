/*
 * Author: Gregory Haynes 	<greg@greghaynes.net>
 *         Jonathan Harker 	<kjharke@cs.pdx.edu>
 *         Spencer Krum		<krum.spencer@gmail.com>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2010, Gregory Haynes
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

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#include <stdint.h>

#include "cpu/cpu.h"
#include "systick/systick.h"
#include "pwm/pwm16.h"
#include "esc/esc.h"
#include "uart/uart.h"
#include "adc/adc.h"
#include "sensors/gyro.h"
#include "sensors/accelero.h"
#include "kalman/kalman.h"
#include "control/motor.h"
#include "control/state.h"

#define DEBUG 1

void handleControlInput(void)
{
	char ch;

	ch = uartRxBufferRead();
	if(ch == '.')
	{
#if DEBUG
		uartSend(" --- GOT . ---", strlen(" --- GOT . ---"));
#endif
		motorsThrustIncreaseAll(100);
	}
	else if(ch == '-')
	{
#if DEBUG
		uartSend(" --- GOT - ---", strlen(" --- GOT - ---"));
#endif
		motorsThrustIncreaseAll(-100);
	}

	motorsSyncDutyCycle();
}

#if 0
static int cnt;
static float roll_sum, pitch_sum, yaw_sum;
static float roll_avg, pitch_avg, yaw_avg;

char buff[100];
void debugState(struct task_t *task)
{
	struct state_controller_t *sc;
	sc = stateControllerGet();
	cnt++;
	roll_sum += sc->roll.gyro.val;
	pitch_sum += sc->pitch.gyro.val;
	yaw_sum += sc->yaw.gyro.val;
	roll_avg = roll_sum / cnt;
	pitch_avg = pitch_sum / cnt;
	yaw_avg = pitch_sum / cnt;
	sprintf(buff, "%f\t%f\t%f\r\n", roll_avg, pitch_avg, yaw_avg);
	uartSend(buff, strlen(buff));
}
#endif

#if 0
char buff[100];
void debugState(struct task_t *task)
{
	struct state_controller_t *sc;
	sc = stateControllerGet();
	sprintf(buff, "%f\t%f\t%f\r\n", sc->roll.angle, sc->pitch.angle, sc->yaw.angle);
	uartSend(buff, strlen(buff));
}
#endif

#if 1
char buff[100];
void debugState(struct task_t *task)
{
	struct state_controller_t *sc;
	sc = stateControllerGet();
	sprintf(buff, "%u\t%u\t%u\r\n", sensorGetAdcVal(&sc->accelero.x), sensorGetAdcVal(&sc->accelero.y), sensorGetAdcVal(&sc->accelero.z));
	uartSend(buff, strlen(buff));
}
#endif

int main(void)
{
	cpuInit();
	systickInit(1);
	uartInit(9600);

	motorsInit();
	//motorsStart();

	stateInit();

	sensorsStart();

	stateStart();

	struct task_t state_debug_task;
	state_debug_task.handler = debugState;
	state_debug_task.msecs = 200;
	tasks_add_task(&state_debug_task);

	while(1)
		tasks_loop();

	return 0;
}
