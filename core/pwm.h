/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef CORE_PWM_H
#define CORE_PWM_H

typedef enum PWM_TIMER {
	PWM_TIMER_16_0 = 1,
	PWM_TIMER_16_1 = 2
} PWM_TIMER;

typedef enum PWM_PIN_16 {
	PWM_PIN_16_0_0 = 1,
    PWM_PIN_16_0_1 = 2,
    PWM_PIN_16_0_2 = 4,
    PWM_PIN_16_1_0 = 16,
    PWM_PIN_16_1_1 = 32,
} PWM_PIN_16;

void pwm_16_init_timers(PWM_TIMER timers);
void pwm_16_init_pins(PWM_PIN_16 pins);
void pwm_16_start_timers(PWM_TIMER timers);
void pwm_16_set_timer_prescaler(PWM_TIMER timer, unsigned int value);
void pwm_16_set_timer_frequency_in_ticks(PWM_TIMER timer, unsigned int value);
void pwm_16_set_duty_cycle_in_ticks(PWM_PIN_16 pins, unsigned int value);

#endif
