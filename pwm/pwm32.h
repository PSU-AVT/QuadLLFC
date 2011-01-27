/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2010-2011, Gregory Haynes
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

#ifndef _PWM32_H_
#define _PWM32_H_

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
 * Returns -1 on error
 */
int pwm32Init(int timer, int pins);

int pwm32Start(int timer);

/**
 * Duty cycle is freqTicks - dutyCycleTicks
 *
 * Returns -1 on error
 */
int pwm32SetDutyCycleInTicks(int pin, uint32_t ticks);

/**
 * Returns -1 on error
 */
int pwm32SetFrequencyInTicks(uint32_t ticks);

#endif
