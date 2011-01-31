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

#include "pwm32.h"

static uint32_t pwmPulseWidth = CFG_PWM_DEFAULT_PULSEWIDTH;
static uint32_t pwmDutyCycle = CFG_PWM_DEFAULT_DUTYCYCLE;

int pwm32EnablePins(int pins);

int pwm32Init(int timer, int pins)
{
	int ret = 0;
	if(timer == PWM32_TIMER0)
	{
		/* Enable the clock for CT32B0 */
		SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT32B0;

		/* Enable specified pins */
		if(-1 == pwm32EnablePins(pins))
		{
			SCB_SYSAHBCLKCTRL &= ~SCB_SYSAHBCLKCTRL_CT32B0;
			ret = -1;
		}

		/* Enable PWM0 */
		TMR_TMR32B0PWMC |= TMR_TMR32B0PWMC_PWM0_ENABLED;

		/* Set default pulse width */
		TMR_TMR32B0MR3 = pwmPulseWidth;

		/* Set default duty cycle */
		TMR_TMR32B0MR0 = (pwmPulseWidth * (100 - pwmDutyCycle)) / 100;

		/* Configure match control register to reset on MR3 */
		TMR_TMR32B0MCR = (TMR_TMR32B0MCR_MR3_RESET_ENABLED);

		/* External Match Register Settings for PWM */
		TMR_TMR32B0EMR = TMR_TMR32B0EMR_EMC0_TOGGLE | TMR_TMR32B0EMR_EM0;

		/* Disable Timer0 by default (enabled by pwmStart of pwmStartFixed) */
		TMR_TMR32B0TCR &= ~TMR_TMR32B0TCR_COUNTERENABLE_MASK;

		/* Make sure that the timer interrupt is enabled */
		NVIC_EnableIRQ(TIMER_32_0_IRQn);
	}
	else if(timer == PWM32_TIMER1)
	{
		/* Enable the clock for CT32B1 */
		SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT32B1;

		/* Enable specified pins */
		if(-1 == pwm32EnablePins(pins))
		{
			SCB_SYSAHBCLKCTRL &= ~SCB_SYSAHBCLKCTRL_CT32B1;
			ret = -1;
		}

		/* Set default pulse width */
		TMR_TMR32B1MR3 = pwmPulseWidth;

		/* Set default duty cycle */
		TMR_TMR32B1MR0 = (pwmPulseWidth * (100 - pwmDutyCycle)) / 100;

		/* Configure match control register to reset on MR3 */
		TMR_TMR32B1MCR = (TMR_TMR32B1MCR_MR3_RESET_ENABLED);

		/* External Match Register Settings for PWM */
		TMR_TMR32B1EMR = TMR_TMR32B1EMR_EMC0_TOGGLE | TMR_TMR32B1EMR_EM0;

		/* Disable Timer1 by default (enabled by pwmStart of pwmStartFixed) */
		TMR_TMR32B1TCR &= ~TMR_TMR32B1TCR_COUNTERENABLE_MASK;

		/* Make sure that the timer interrupt is enabled */
		NVIC_EnableIRQ(TIMER_32_1_IRQn);
	}
	else
		return -1;
	return ret;
}

int pwm32EnablePins(int pins)
{
	int ret = 0;
	if(pins & PWM32_PIN0_0)
	{
		/* Configure PIN 1.6 as Timer32B0 MAT0 output */
		IOCON_PIO1_6 &= ~IOCON_PIO1_6_FUNC_MASK;
		IOCON_PIO1_6 |= IOCON_PIO1_6_FUNC_CT32B0_MAT0;

		/* Enable PWM0 */
		TMR_TMR32B0PWMC |= TMR_TMR32B0PWMC_PWM0_ENABLED;
	}
	if(pins & PWM32_PIN0_1)
	{
		/* Configure PIN 1.7 as Timer32B0 MAT1 output */
		IOCON_PIO1_7 &= ~IOCON_PIO1_7_FUNC_MASK;
		IOCON_PIO1_7 |= IOCON_PIO1_7_FUNC_CT32B0_MAT1;

		/* Enable PWM0 */
		TMR_TMR32B0PWMC |= TMR_TMR32B0PWMC_PWM1_ENABLED;
	}
	if(pins & PWM32_PIN0_2)
	{
		/* Configure PIN 0.1 as Timer32B0 MAT2 output */
		IOCON_PIO0_1 &= ~IOCON_PIO0_1_FUNC_MASK;
		IOCON_PIO0_1 |= IOCON_PIO0_1_FUNC_CT32B0_MAT2;

		/* Enable PWM2 */
		TMR_TMR32B0PWMC |= TMR_TMR32B0PWMC_PWM2_ENABLED;
	}
	if(pins & PWM32_PIN0_3)
	{
		/* This is a JTAG pin */
		ret = -1;
	}
	if(pins & PWM32_PIN1_0)
	{
		/* This is a JTAG pin */
		ret = -1;
	}
	if(pins & PWM32_PIN1_1)
	{
		/* This is a JTAG pin */
		ret = -1;
	}
	if(pins & PWM32_PIN1_2)
	{
		/* This is a SWD pin */
		ret = -1;
	}
	if(pins & PWM32_PIN1_3)
	{
		/* Configure PIN 0.1 as Timer32B1 MAT3 output */
		IOCON_PIO1_4 &= ~IOCON_PIO1_4_FUNC_MASK;
		IOCON_PIO1_4 |= IOCON_PIO1_4_FUNC_CT32B1_MAT3;

		/* Enable PWM3 */
		TMR_TMR32B1PWMC |= TMR_TMR32B1PWMC_PWM3_ENABLED;
	}

	return ret;
}

int pwm32Start(int timer)
{
	if(timer == PWM32_TIMER0)
	{
		/* Disable interrupt on MR3 in case it was enabled by pwmStartFixed() */
		TMR_TMR32B0MCR  &= ~(TMR_TMR32B0MCR_MR3_INT_MASK);

		/* Enable Timer0 */
		TMR_TMR32B0TCR = TMR_TMR32B0TCR_COUNTERENABLE_ENABLED;
	}
	else if(timer == PWM32_TIMER1)
	{
		/* Disable interrupt on MR3 in case it was enabled by pwmStartFixed() */
		TMR_TMR32B1MCR  &= ~(TMR_TMR32B1MCR_MR3_INT_MASK);

		/* Enable Timer1 */
		TMR_TMR32B1TCR = TMR_TMR32B1TCR_COUNTERENABLE_ENABLED;
	}
	else
		return -1;
	return 0;
}

int pwm32SetDutyCycleInTicks(int pin, uint32_t ticks)
{
  if (ticks > pwmPulseWidth)
  {
    /* Duty Cycle must be a value between 1 and pulseWidth */
    return -1;
  }

  switch(pin)
  {
  case PWM32_PIN0_0:
	  TMR_TMR32B0MR0 = pwmPulseWidth - ticks;
	  break;
  case PWM32_PIN0_1:
	  TMR_TMR32B0MR1 = pwmPulseWidth - ticks;
	  break;
  case PWM32_PIN0_2:
	  TMR_TMR32B0MR2 = pwmPulseWidth - ticks;
	  break;
  case PWM32_PIN0_3:
	  return -1;
  case PWM32_PIN1_0:
	  return -1;
  case PWM32_PIN1_1:
	  return -1;
  case PWM32_PIN1_2:
	  return -1;
  case PWM32_PIN1_3:
	  TMR_TMR32B1MR3 = pwmPulseWidth - ticks;
	  break;
  default:
	  return -1;
  }

  return 0;
}

int pwm32SetFrequencyInTicks(uint32_t ticks)
{
  if (ticks < 1)
  {
    return -1;
  }

  /* Set Pulse Width (MR3)*/
  TMR_TMR32B0MR3 = (pwmPulseWidth = ticks);

  /* Adjust Duty Cycle (MR0) */
  TMR_TMR32B0MR0 = (pwmPulseWidth * (100 - pwmDutyCycle)) / 100;

  return 0;
}
