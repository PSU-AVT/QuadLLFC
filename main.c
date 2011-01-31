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
#include "pwm/pwm.h"

#if 1
int main(void)
{
	cpuInit();

	/* Enable the clock for CT16B0 */
	SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT16B0;

	IOCON_PIO0_8 &= ~IOCON_PIO0_8_FUNC_MASK;
	IOCON_PIO0_8 |= IOCON_PIO0_8_FUNC_CT16B0_MAT0;

	/* Set pulse width */
	TMR_TMR16B0MR3 = 60000;

	/* Set duty cycle */
	TMR_TMR16B0MR0 = 60000-24000;

	TMR_TMR16B0MCR = (TMR_TMR16B0MCR_MR3_RESET_ENABLED);

	/* External Match Register Settings for PWM */
	TMR_TMR16B0EMR = TMR_TMR16B0EMR_EMC0_TOGGLE | TMR_TMR16B0EMR_EM0;

	TMR_TMR16B0TCR &= ~TMR_TMR16B0TCR_COUNTERENABLE_MASK;

	/* Enable PWM0 */
	TMR_TMR16B0PWMC |= TMR_TMR16B0PWMC_PWM0_ENABLED | TMR_TMR16B0PWMC_PWM3_ENABLED;;

	/* Make sure that the timer interrupt is enabled */
	NVIC_EnableIRQ(TIMER_16_0_IRQn);

	TMR_TMR16B0MCR  &= ~(TMR_TMR16B0MCR_MR3_INT_MASK);
	TMR_TMR16B0TCR = TMR_TMR16B0TCR_COUNTERENABLE_ENABLED;

	TMR_TMR16B0PR = 2;

	systickInit(1);

	systickDelay(10000);

	while(1)
	{
		systickDelay(1);
		TMR_TMR16B0MR0--;
	}

	return 0 ;
}
#else

int main(void)
{

	pwmInit();
	pwmStart();

	while(1);
	return 0;
}

#endif
