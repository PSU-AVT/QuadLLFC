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

#include "esc.h"

static struct esc_controller_t _esc_control;
static int _esc_initialized;

void escInit(struct esc_t *esc)
{
	pwm16InitTimers(esc->pwm_timer);
	pwm16InitPins(esc->pwm_pin);
	escSetDutyCycle(esc, esc->duty_cycle);
}

struct esc_controller_t *escGetController(void)
{
	return &_esc_control;
}

void escsInit(void)
{
	int i;
	if(!_esc_initialized)
	{
		for(i = 0;i < ESC_CNT;i++)
			escInit(&_esc_control.escs[i]);
	}
}

void escsArm(void)
{
}

void escSetDutyCycle(struct esc_t *esc, uint16_t cycle)
{
}
