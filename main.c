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
#include "timer32/timer32.h"
#include "adc/adc.h"

extern volatile uint32_t timer32_0_counter; // In timer32.c

#define ESC_STARTUP_CYCLE 35000

void setupEscs(void)
{
	struct esc_controller_t *controller;

	controller = escGetController();
	ESC_SETUP(controller->escs[0],
	          ESC_0_PWM_PIN,
	          ESC_0_PWM_TIMER,
	          0)
	ESC_SETUP(controller->escs[1],
	          ESC_1_PWM_PIN,
	          ESC_1_PWM_TIMER,
	          0)
	ESC_SETUP(controller->escs[2],
	          ESC_2_PWM_PIN,
	          ESC_2_PWM_TIMER,
	          0)
	ESC_SETUP(controller->escs[3],
	          ESC_3_PWM_PIN,
	          ESC_3_PWM_TIMER,
	          0)
	escsInit();
	escsArm();
}

int main(void)
{
	struct esc_controller_t *controller;

	cpuInit();
	systickInit(1);

	uartInit(9600);
	uartSend("Arming\n", 7);

	setupEscs();
	uartSend("Armed\n", 6);

	controller = escGetController();

	escSetDutyCycle(&(controller->escs[0]),
	                ESC_STARTUP_CYCLE);
	escSetDutyCycle(&(controller->escs[1]),
	                ESC_STARTUP_CYCLE);
	escSetDutyCycle(&(controller->escs[2]),
	                ESC_STARTUP_CYCLE);
	escSetDutyCycle(&(controller->escs[3]),
	                ESC_STARTUP_CYCLE);


	adcInit(ADC_PIN0);
	adcSelectPins(ADC_PIN0);
	adcStart();

	uint16_t val = 0;
	uint8_t low, high;

	while(1)
	{
		systickDelay(100);
		uartSendByte('.');
		val = adcGetVal(ADC_PIN0);
		low = ((uint8_t*)(&val)[0]);
		high = ((uint8_t*)(&val))[1];
		uartSendByte(high);
		uartSendByte(low);
	}

	int speed;
	uint8_t read;
	while(1)
	{
			while(uartRxBufferDataPending())
			{
				read = uartRxBufferRead();
				if(read == '.')
				{
					speed = -400;
				}
				else if(read == '-')
				{
					speed = 400;
				}
				else
				{
					speed = 0;
				}

				if(speed)
				{
					escSetDutyCycle(&(controller->escs[0]),
									controller->escs[0].duty_cycle+speed);
					escSetDutyCycle(&(controller->escs[1]),
									controller->escs[1].duty_cycle+speed);
					escSetDutyCycle(&(controller->escs[2]),
									controller->escs[2].duty_cycle+speed);
					escSetDutyCycle(&(controller->escs[3]),
									controller->escs[3].duty_cycle+speed);
				}
				else
				{
					escSetDutyCycle(&(controller->escs[0]),
					                ESC_STARTUP_CYCLE);
					escSetDutyCycle(&(controller->escs[1]),
					                ESC_STARTUP_CYCLE);
					escSetDutyCycle(&(controller->escs[2]),
					                ESC_STARTUP_CYCLE);
					escSetDutyCycle(&(controller->escs[3]),
					                ESC_STARTUP_CYCLE);
				}
			}
	}
	return 0;
}
