/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "esc.h"
#include "core/pwm.h"
#include "core/systick.h"

//Need to find out what 100% throttle on the esc coresponds to what pulse width
#define ESC_PWM_MAX 20000 //Gives a pwm width of 1.67 ms
//#define ESC_PWM_MAX 12000 //Gives a pwm width of 2.0ms
#define ESC_PWM_MIN 36000
#define ESC_PWM_PRESCALE 2
#define ESC_PWM_FREQUENCY 60000
#define ESC_ARM_DUTY_CYCLE 36000

static float throttle_to_pwm_factor = ESC_PWM_MAX - ESC_PWM_MIN;

//#define ESC_CNT 4

typedef struct esc {
	float throttle; // Between 0 and 1 where 1 is 100%
	float scaled_throttle;
	PWM_PIN_16 pwm_pin;
} esc;

static esc _escs[ESC_CNT];

void esc_init(ESC_LABEL ndx) {
	_escs[ndx].throttle = 0;
	_escs[ndx].scaled_throttle = 0;
	switch(ndx) {
	case ESC_FRONT:
		_escs[ndx].pwm_pin = PWM_PIN_16_0_0;
		break;
	case ESC_LEFT:
		_escs[ndx].pwm_pin = PWM_PIN_16_0_1;
		break;
	case ESC_BACK:
		_escs[ndx].pwm_pin = PWM_PIN_16_1_0;
		break;
	case ESC_RIGHT:
		_escs[ndx].pwm_pin = PWM_PIN_16_1_1;
		break;
	}
	pwm_16_init_pins(_escs[ndx].pwm_pin);
}

void esc_init_all(void) {
	// Initialize both 16bit timers to PWM mode
	pwm_16_init_timers(PWM_TIMER_16_0 | PWM_TIMER_16_1);

	// Set PWM prescaler
	pwm_16_set_timer_prescaler(PWM_TIMER_16_0 | PWM_TIMER_16_1, ESC_PWM_PRESCALE);

	// Set PWM frequency
	pwm_16_set_timer_frequency_in_ticks(PWM_TIMER_16_0 | PWM_TIMER_16_1, ESC_PWM_FREQUENCY);

	int i;
	for(i = 0;i < ESC_CNT;++i)
		esc_init(i);
}

void esc_update_pwm(ESC_LABEL esc) {
	unsigned int pwm_val = ESC_PWM_MIN + (_escs[esc].scaled_throttle * throttle_to_pwm_factor);
	pwm_16_set_duty_cycle_in_ticks(_escs[esc].pwm_pin, pwm_val);
}

void esc_rescale_all() {
	int max_ndx=0, i;
        float scale_fact = 0.0;
	for(i = 1;i < ESC_CNT;++i) {
		if(_escs[max_ndx].throttle <= _escs[i].throttle)
			max_ndx = i;
	}

	// If we don't need to rescale set the scale factor to 1 so the 
        // scaled_throttle field will get correctly filled.
	if(_escs[max_ndx].throttle <= 1)
                scale_fact = 1.0;
        else 
                scale_fact = 1.0 / _escs[max_ndx].throttle;
                

	for(i = 0; i < ESC_CNT;++i)
		_escs[i].scaled_throttle = _escs[i].throttle * scale_fact;
}

void esc_set_throttle(ESC_LABEL esc, float value) {
	if(value < 0) // negatives dont make sense here
		return;
	_escs[esc].throttle = value;
	esc_rescale_all();
	esc_update_pwm(esc);
}

void esc_set_all_throttles(float *values) {
	int i;
	for(i = 0;i < ESC_CNT;++i) {
		if(values[i] < 0)
			continue;
		_escs[i].throttle = values[i];
	}

	esc_rescale_all();

	for(i = 0;i < ESC_CNT;++i)
		esc_update_pwm(i);
}

void esc_arm_all(void) {
	int i;
	for(i = 0;i < ESC_CNT;++i)
		esc_set_throttle(i, 0);

	pwm_16_start_timers(PWM_TIMER_16_0 | PWM_TIMER_16_1);
}
