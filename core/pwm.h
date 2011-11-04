#ifndef CORE_PWM_H
#define CORE_PWM_H

typedef enum PWM_TIMER {
	PWM_TIMER_16_0 = 1,
	PWM_TIMER_16_1 = 2
} PWM_TIMER;

typedef enum PWM_PIN {
	PWM_PIN_16_0_0 = 1,
    PWM_PIN_16_0_1 = 2,
    PWM_PIN_16_0_2 = 4,
    PWM_PIN_16_1_0 = 16,
    PWM_PIN_16_1_1 = 32,
} PWM_PIN;

void pwm_16_init_timers(PWM_TIMER timers);

#endif
