/*
 * Author: Gregory Haynes <greg@greghaynes.net>
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

void sendHello(struct task_t *task)
{
	uartSend("Hello\r\n", 7);
}

int main(void)
{
	cpuInit();
	systickInit(1);
	uartInit(9600);

	motorsInit();
	motorsStart();

	stateInit();

	sensorsStart();

	stateStart();

	struct task_t hello_task;
	hello_task.handler = sendHello;
	hello_task.msecs = 1000;
	tasks_add_task(&hello_task);

	while(1)
		tasks_loop();

	return 0;
}
