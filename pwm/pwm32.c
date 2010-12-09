#include "pwm32.h"

uint32_t pwmPulseWidth = CFG_PWM_DEFAULT_PULSEWIDTH;
uint32_t pwmDutyCycle = CFG_PWM_DEFAULT_DUTYCYCLE;


/**
 * Returns -1 on failure, 0 on success
 */
int pwm32EnablePins(int pins);

int pwm32Init(int timer, int pins)
{
	if(timer == PWM32_TIMER0)
	{
		/* Enable the clock for CT32B0 */
		SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT32B0;

		/* Enable specified pins */
		if(-1 == pwm32EnablePins(pins))
		{
			SCB_SYSAHBCLKCTRL &= ~SCB_SYSAHBCLKCTRL_CT32B0;
			return -1;
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
			return -1;
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
	return 0;
}

int pwm32EnablePins(int pins)
{
	if(pins & PWM32_PIN0_0)
	{
		/* Configure PIN 1.6 as Timer32B0 MAT0 output */
		IOCON_PIO1_6 &= ~IOCON_PIO1_6_FUNC_MASK;
		IOCON_PIO1_6 |= IOCON_PIO1_6_FUNC_CT32B0_MAT0;

		/* Enable PWM0 */
		TMR_TMR32B0PWMC |= TMR_TMR32B0PWMC_PWM0_ENABLED;
	}
	return 0;
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

int pwm32SetDutyCycleInTicks(uint32_t ticks)
{
  if (ticks > pwmPulseWidth)
  {
    /* Duty Cycle must be a value between 1 and pulseWidth */
    return -1;
  }

  /* Set Duty Cycle (MR0) */
  TMR_TMR32B0MR0 = pwmPulseWidth - ticks;

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
