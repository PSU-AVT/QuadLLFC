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

/*
 * Usage:
 *   obtain esc_controller_t from escGetController();
 *   Fill out esc's in controller with correct pin, timer, and duty cycle values
 *     You probably want 0 duty cycle to prevent arming early
 *   Call escsInit();
 *   Call escsArm();
 */

#ifndef _ESC_H_
#define _ESC_H

#include <stdint.h>

#include "../config.h"
#include "../pwm/pwm16.h"

#define ESC_0_PWM_PIN PWM16_PIN0_0
#define ESC_1_PWM_PIN PWM16_PIN0_1
#define ESC_2_PWM_PIN PWM16_PIN1_0
#define ESC_3_PWM_PIN PWM16_PIN1_1

#define ESC_0_PWM_TIMER PWM16_TIMER0
#define ESC_1_PWM_TIMER PWM16_TIMER0
#define ESC_2_PWM_TIMER PWM16_TIMER0
#define ESC_3_PWM_TIMER PWM16_TIMER1

struct esc_t
{
	int pwm_pin;
	int pwm_timer;
	uint16_t duty_cycle;
};

struct esc_controller_t
{
	struct esc_t escs[CFG_ESC_CNT];
};

#define ESC_SETUP(ESC, PIN, TIMER, CYCLE) \
	ESC.pwm_pin = PIN;\
	ESC.pwm_timer = TIMER;\
	ESC.duty_cycle = CYCLE;

struct esc_controller_t *escGetController(void);
void escInit(struct esc_t *esc);
void escsInit(void);
void escsArm(void);
void escSetDutyCycle(struct esc_t *esc, uint16_t cycle);

#endif
