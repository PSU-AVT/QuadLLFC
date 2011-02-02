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

#include "cpu/cpu.h"
#include "systick/systick.h"
#include "pwm/pwm16.h"
#include "esc/esc.h"

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
	cpuInit();
	systickInit(1);

	struct esc_controller_t *controller;

	controller = escGetController();

	/*
	pwm16InitPins(PWM16_PIN1_0);
	pwm16InitTimers(PWM16_TIMER1);
	pwm16SetFrequencyInTicks(PWM16_TIMER1, 60000);
	pwm16SetDutyCycleInTicks(PWM16_PIN1_0, 30000);
	pwm16SetTimerPrescaler(PWM16_TIMER1, 2);
	pwm16StartTimers(PWM16_TIMER1);
	*/

	setupEscs();

	while(1)
	{
		systickDelay(10);
		escSetDutyCycle(&(controller->escs[0]),
		                controller->escs[0].duty_cycle+1);
		escSetDutyCycle(&(controller->escs[1]),
                        controller->escs[1].duty_cycle+1);
	}

	return 0;
}
