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

extern volatile uint32_t timer32_0_counter; // In timer32.c

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

void doResponseUpdate(void)
{
}

int main(void)
{
	cpuInit();
	systickInit(1);
	uartInit(9600);

	struct state_controller_t *sc = stateControllerGet();

	// gyro init
	struct gyro3d_t gyros;
	gyro3dInit(&gyros, ADC_PIN0, ADC_PIN1, ADC_PIN2);
	sensorsStart();

	motorsInit();
	motorsStart();

	// Start sensors
	stateInit();
	//systickDelay(10000); // Let adc settle
	gyro3dStart(&gyros);

	// Ticks when actions should be performed
	uint32_t state_update_ticks = systickGetTicks();
	uint32_t response_update_ticks = state_update_ticks;
#if DEBUG
	uint32_t state_output_ticks = state_update_ticks;
#endif
	uint32_t cur_ticks;

	uint16_t last_state_update = state_update_ticks;
	float state_update_dt;

	char buff[50];

	// Main control loop
	while(1)
	{
		cur_ticks = systickGetTicks();

		// Check for control input
		if(uartRxBufferDataPending())
			handleControlInput();

		// Check for state update
		if(cur_ticks >= state_update_ticks)
		{
			state_update_dt = (cur_ticks - last_state_update) * .001;
			stateUpdateFromGyros(&gyros, state_update_dt);
			last_state_update = cur_ticks;
			state_update_ticks += CFG_STATE_UPDATE_MSECS;
		}

		// Check for response update
		if(cur_ticks >= response_update_ticks)
		{
			doResponseUpdate();
			response_update_ticks += CFG_RESPONSE_UPDATE_MSECS;
		}

#if DEBUG
		if(cur_ticks >= state_output_ticks)
		{
			sprintf(buff, "Roll: %f %u %f\r\n",
			       sc->x.angle, sensorGetAdcVal(&gyros.roll.sensor), gyroGetAngVel(&gyros.roll));
			uartSend(buff, strlen(buff));
			state_output_ticks += CFG_STATE_OUTPUT_MSECS;
		}
#endif

	}

	return 0;
}
