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
		pwm32EnablePins(pins);

		/* Set default pulse width */
		TMR_TMR32B0MR3 = pwmPulseWidth;

		/* Set default duty cycle */
		TMR_TMR32B0MR0 = (pwmPulseWidth * (100 - pwmDutyCycle)) / 100;

		/* Configure match control register to reset on MR3 */
		TMR_TMR32B0MCR = (TMR_TMR32B0MCR_MR3_RESET_ENABLED);

		/* External Match Register Settings for PWM */
		TMR_TMR32B0EMR = TMR_TMR32B0EMR_EMC0_TOGGLE | TMR_TMR32B0EMR_EM0;
	}
	else if(timer == PWM32_TIMER1)
	{
		/* Enable the clock for CT32B1 */
		SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT32B1;

		/* Enable specified pins */
		pwm32EnablePins(pins);

		/* Set default pulse width */
		TMR_TMR32B1MR3 = pwmPulseWidth;

		/* Set default duty cycle */
		TMR_TMR32B1MR0 = (pwmPulseWidth * (100 - pwmDutyCycle)) / 100;

		/* Configure match control register to reset on MR3 */
		TMR_TMR32B1MCR = (TMR_TMR32B1MCR_MR3_RESET_ENABLED);

		/* External Match Register Settings for PWM */
		TMR_TMR32B1EMR = TMR_TMR32B1EMR_EMC0_TOGGLE | TMR_TMR32B1EMR_EM0;
	}
	else
		return -1;
	return 0;
}

int pwm32EnablePins(int pins)
{
	if(pins & PWM32_PIN0_0)
	{
		IOCON_PIO1_6 &= ~IOCON_PIO1_6_FUNC_MASK;
		IOCON_PIO1_6 |= IOCON_PIO1_6_FUNC_CT32B0_MAT0;
		TMR_TMR32B0PWMC |= TMR_TMR32B0PWMC_PWM0_ENABLED;
	}
	return 0;
}
