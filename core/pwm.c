/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "pwm.h"

#include "lpc134x.h"

void pwm_16_init_timers(PWM_TIMER timers) {
    if(timers & PWM_TIMER_16_0) {
        /* Enable Timer */
    	SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT16B0;

    	/* Match compare register */
    	TMR_TMR16B0MCR = TMR_TMR16B0MCR_MR3_RESET_ENABLED;

        /* External Match Register Settings for PWM */
    	TMR_TMR16B0EMR = TMR_TMR16B0EMR_EMC0_TOGGLE | TMR_TMR16B0EMR_EM0;

        /* Disable counter */
    	TMR_TMR16B0TCR &= TMR_TMR16B0TCR_COUNTERENABLE_MASK;
    }
    if(timers & PWM_TIMER_16_1) {
		/* Enable Timer */
		SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT16B1;

		TMR_TMR16B1MCR = (TMR_TMR16B1MCR_MR3_RESET_ENABLED);

		/* External Match Register Settings for PWM */
		TMR_TMR16B1EMR = TMR_TMR16B1EMR_EMC0_TOGGLE | TMR_TMR16B1EMR_EM0;

		/* Disable counter */
		TMR_TMR16B1TCR &= ~TMR_TMR16B1TCR_COUNTERENABLE_MASK;
    }
}

void pwm_16_init_pins(PWM_PIN_16 pins) {
    if(pins & PWM_PIN_16_0_0) {
		IOCON_PIO0_8 &= ~IOCON_PIO0_8_FUNC_MASK;
		IOCON_PIO0_8 |= IOCON_PIO0_8_FUNC_CT16B0_MAT0;

		TMR_TMR16B0PWMC |= TMR_TMR16B0PWMC_PWM0_ENABLED;
    }

    if(pins & PWM_PIN_16_0_1) {
		IOCON_PIO0_9 &= ~IOCON_PIO0_9_FUNC_MASK;
		IOCON_PIO0_9 |= IOCON_PIO0_9_FUNC_CT16B0_MAT1;

		TMR_TMR16B0PWMC |= TMR_TMR16B0PWMC_PWM1_ENABLED;
    }

    // PIN_16_0_2 overrides JTAG and therefore is disabled

    if(pins & PWM_PIN_16_1_0) {
		IOCON_PIO1_9 &= ~IOCON_PIO1_9_FUNC_MASK;
		IOCON_PIO1_9 |= IOCON_PIO1_9_FUNC_CT16B1_MAT0;

		TMR_TMR16B1PWMC |= TMR_TMR16B1PWMC_PWM0_ENABLED;
    }

    if(pins & PWM_PIN_16_1_1) {
		IOCON_PIO1_10 &= ~IOCON_PIO1_10_FUNC_MASK;
		IOCON_PIO1_10 |= IOCON_PIO1_10_FUNC_CT16B1_MAT1;

		TMR_TMR16B1PWMC |= TMR_TMR16B1PWMC_PWM1_ENABLED;
    }
}

void pwm_16_start_timers(PWM_TIMER timers) {
	if(timers & PWM_TIMER_16_0) {
		TMR_TMR16B0MCR  &= ~(TMR_TMR16B0MCR_MR3_INT_MASK);
		TMR_TMR16B0TCR = TMR_TMR16B0TCR_COUNTERENABLE_ENABLED;
	}
	if(timers & PWM_TIMER_16_1) {
		TMR_TMR16B1MCR  &= ~(TMR_TMR16B1MCR_MR3_INT_MASK);
		TMR_TMR16B1TCR = TMR_TMR16B1TCR_COUNTERENABLE_ENABLED;
	}
}

void pwm_16_set_timer_prescaler(PWM_TIMER timer, unsigned int value) {
    if(timer & PWM_TIMER_16_0)
		TMR_TMR16B0PR = value;
    if(timer & PWM_TIMER_16_1)
		TMR_TMR16B1PR = value;
}

void pwm_16_set_timer_frequency_in_ticks(PWM_TIMER timer, unsigned int value) {
    if(timer & PWM_TIMER_16_0)
		TMR_TMR16B0MR3 = value;
    if(timer & PWM_TIMER_16_1)
		TMR_TMR16B1MR3 = value;
}

void pwm_16_set_duty_cycle_in_ticks(PWM_PIN_16 pins, unsigned int value) {
    if(pins & PWM_PIN_16_0_0)
		TMR_TMR16B0MR0 = value;
    if(pins & PWM_PIN_16_0_1)
    	TMR_TMR16B0MR1 = value;
    if(pins & PWM_PIN_16_0_2)
    	TMR_TMR16B0MR2 = value;
    if(pins & PWM_PIN_16_1_0)
    	TMR_TMR16B1MR0 = value;
    if(pins & PWM_PIN_16_1_1)
    	TMR_TMR16B1MR1 = value;
}
