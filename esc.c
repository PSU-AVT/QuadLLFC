/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "esc.h"
#include "core/pwm.h"
#include "core/systick.h"
#include "commands.h"

//1.92ms coresponds to 100% Throttle with our esc's.
#define ESC_PWM_MAX 14000 //Gives a pwm width of 1.916ms. 
#define ESC_PWM_MIN 36000
#define ESC_PWM_PRESCALE 2
#define ESC_PWM_FREQUENCY 60000
#define ESC_ARM_DUTY_CYCLE 36000

static float throttle_to_pwm_factor = ESC_PWM_MAX - ESC_PWM_MIN;
static uint32_t _send_state_ticks = 500;

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
	esc_set_throttle(ESC_FRONT, 0);
	esc_set_throttle(ESC_LEFT, 0);
	esc_set_throttle(ESC_BACK, 0);
	esc_set_throttle(ESC_RIGHT, 0);
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
	// Wait for arming
	systickDelay(6000);
}

void esc_send_state(void) {
	float throttles[4];
	throttles[0] = _escs[0].throttle;
	throttles[1] = _escs[1].throttle;
	throttles[2] = _escs[2].throttle;
	throttles[3] = _escs[3].throttle;

	command_send(COMMAND_MOTORS_STATE, (unsigned char*)throttles, sizeof(float)*4);
}

static uint32_t _last_send_state_ticks;

void esc_update(void) {
	if((systickGetTicks() - _last_send_state_ticks) >= _send_state_ticks) {
		esc_send_state();
		_last_send_state_ticks = systickGetTicks();
	}
}

