#ifndef _PWM_H_
#define _PWM_H_

#include "../config.h"

enum PWM32_TIMER_T {
	PWM32_TIMER0,
	PWM32_TIMER1,
};

/* PINX can only be used with TIMERX */
enum PWM32_PIN_T {
	PWM32_PIN0_0 = 1,
	PWM32_PIN0_1 = 2,
	PWM32_PIN0_2 = 4,
	PWM32_PIN0_3 = 8,
	PWM32_PIN1_0 = 16,
	PWM32_PIN1_1 = 32,
	PWM32_PIN1_2 = 64,
	PWM32_PIN1_3 = 128
};

/**
 * Returns -1 on failure, 0 on success
 */
int pwm32Init(int timer, int pins);

int pwm32Start(int timer);

int pwm32SetDutyCycleInTicks(uint32_t ticks);

int pwm32SetFrequencyInTicks(uint32_t ticks);

#endif
