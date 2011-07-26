/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Gregory Haynes
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

#include "motor.h"
#include "../esc/esc.h"

static struct motor_controller_t _motor_controller;
static float motor_val_pwm_fact;

void motorsSyncDutyCycle(void);

struct motor_controller_t *motorControllerGet(void)
{
	return &_motor_controller;
}

void motorsInit(void)
{
	motor_val_pwm_fact = ((float)CFG_MOTOR_MIN_THRUST - CFG_MOTOR_MAX_THRUST) / CFG_MOTOR_MAX;

	struct esc_controller_t *esc_controller;
	uint8_t i;

	esc_controller = escGetController();

	for(i = 0;i < CFG_MOTOR_CNT;i++)
		_motor_controller.motors[i] = 0;

	ESC_SETUP(esc_controller->escs[0],
	          ESC_1_PWM_PIN,
	          ESC_1_PWM_TIMER,
	          CFG_MOTOR_DEFAULT_DUTY_CYCLE)
	ESC_SETUP(esc_controller->escs[1],
	          ESC_0_PWM_PIN,
	          ESC_0_PWM_TIMER,
	          CFG_MOTOR_DEFAULT_DUTY_CYCLE)
	ESC_SETUP(esc_controller->escs[2],
	          ESC_2_PWM_PIN,
	          ESC_2_PWM_TIMER,
	          CFG_MOTOR_DEFAULT_DUTY_CYCLE)
	ESC_SETUP(esc_controller->escs[3],
	          ESC_3_PWM_PIN,
	          ESC_3_PWM_TIMER,
	          CFG_MOTOR_DEFAULT_DUTY_CYCLE)
	escsInit();
}

void motorsStart(void)
{
	uint8_t i;

	escsArm();

	motorsSyncDutyCycle();
}

// If any motors are over CFG_MOTOR_MAX we rescale all, equally to make
// this not the case
void motors_rescale(float *motor_vals, int *scaled_vals) {
	int i;
	float scale_factor;

	// motor into working array
	for(i = 0;i < CFG_MOTOR_CNT;++i)
		scaled_vals[i] = motor_vals[i];

	// Find max
	int max_ndx = 0;
	for(i = 1;i < CFG_MOTOR_CNT;++i) {
		if(scaled_vals[i] > scaled_vals[max_ndx])
			max_ndx = i;
	}

	// We need to rescale
	if(scaled_vals[max_ndx] > CFG_MOTOR_MAX) {
		scale_factor = ((float)CFG_MOTOR_MAX) / scaled_vals[max_ndx];

		for(i = 0;i < CFG_MOTOR_CNT;++i)
			scaled_vals[i] *= scale_factor;
	}
}

int motor_val_to_pwm(int val) {
	if(val < 0)
		return CFG_MOTOR_MIN_THRUST;
	return CFG_MOTOR_MIN_THRUST - (val * motor_val_pwm_fact);
}

// Called to actually apply motor changes
void motorsSyncDutyCycle(void)
{
	int i;

	// Rescale motors
	int scaled_vals[CFG_MOTOR_CNT];
	motors_rescale(_motor_controller.motors, scaled_vals);

	// Convert to pwm duty cycle
	for(i = 0;i < CFG_MOTOR_CNT;++i) {
		scaled_vals[i] = motor_val_to_pwm(scaled_vals[i]);
	}

	struct esc_controller_t *controller;

	controller = escGetController();
	for(i = 0;i < CFG_MOTOR_CNT;++i)
	{
		escSetDutyCycle(&controller->escs[i],
		                scaled_vals[i]);
	}
}

void motors_off(void)
{
	float vals[4];
	int i;
	for(i = 0;i < CFG_MOTOR_CNT;++i)
		vals[i] = 0;
	motors_set(vals);
}

void motor_set(int ndx, float value)
{
	_motor_controller.motors[ndx] = value;
	motorsSyncDutyCycle();
}

void motors_set(float *values)
{
	uint8_t i;
	for(i = 0;i < CFG_MOTOR_CNT;i++)
		_motor_controller.motors[i] = values[i];
	motorsSyncDutyCycle();
}

float motor_get_val(int ndx)
{
	if(ndx < 0 || ndx >= CFG_MOTOR_CNT)
		return 0;
	return _motor_controller.motors[ndx];
}
