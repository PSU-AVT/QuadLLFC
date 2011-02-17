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

struct motor_controller_t *motorControllerGet(void)
{
	return &_motor_controller;
}

void motorInit(struct motor_t *motor)
{
	motor->duty_cycle = CFG_MOTOR_DEFAULT_DUTY_CYCLE;
	motor->thrust_max = CFG_MOTOR_DEFAULT_MAX_THRUST;
	motor->thrust_min = CFG_MOTOR_DEFAULT_MIN_THRUST;
	motor->thrust_proportion = 1;
}

void motorsInit(void)
{
	struct esc_controller_t *esc_controller;
	uint8_t i;

	for(i = 0;i < CFG_MOTOR_CNT;i++)
		motorInit(&_motor_controller.motors[i]);

	ESC_SETUP(esc_controller->escs[0],
	          ESC_0_PWM_PIN,
	          ESC_0_PWM_TIMER,
	          CFG_MOTOR_DEFAULT_DUTY_CYCLE)
	ESC_SETUP(esc_controller->escs[1],
	          ESC_1_PWM_PIN,
	          ESC_1_PWM_TIMER,
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
	for(i = 0;i < CFG_MOTOR_CNT;i++)
	{
		_motor_controller.motors[i].duty_cycle = _motor_controller.motors[i].thrust_min;
	}
	motorsSyncDutyCycle();
}

void motorsSyncDutyCycle(void)
{
	uint8_t i;
	struct esc_controller_t *controller;

	controller = escGetController();
	for(i = 0;i < CFG_MOTOR_CNT;i++)
	{
		escSetDutyCycle(&controller->escs[i],
		                _motor_controller.motors[i].duty_cycle);
	}
}

void motorThrustIncrease(struct motor_t *motor, float value)
{
	motor->duty_cycle += value * motor->thrust_proportion;
}

void motorsThrustIncreaseAll(float value)
{
	uint8_t i;
	for(i = 0;i < CFG_MOTOR_CNT;i++)
	{
		motorThrustIncrease(&_motor_controller.motors[i],
		                      value);
	}
}
