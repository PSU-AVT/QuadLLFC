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

#include "gyro.h"
#include "../adc/adc.h"

void gyroInit(struct gyro_t *g, uint16_t adc_pin)
{
	sensorInit(&g->sensor, adc_pin);
}

void gyroStart(struct gyro_t *g)
{
	// Block until we get a valid base gyro value
	do
		g->base_val = sensorGetAdcVal(&g->sensor);
	while(g->base_val == ADC_RESULT_INVALID);
}

void gyro3dInit(struct gyro3d_t *g, uint16_t r_adc_pin,
                uint16_t p_adc_pin,
                uint16_t y_adc_pin)
{
	gyroInit(&g->roll, r_adc_pin);
	gyroInit(&g->pitch, p_adc_pin);
	gyroInit(&g->yaw, y_adc_pin);
}

void gyro3dStart(struct gyro3d_t *g)
{
	gyroStart(&g->roll);
	gyroStart(&g->pitch);
	gyroStart(&g->yaw);
}

float gyroGetAngVel(struct gyro_t *g)
{
	float offset = (g->base_val - g->val);
	if(offset < CFG_GYRO_ZERO_WIDTH && offset > (-CFG_GYRO_ZERO_WIDTH))
		return 0;
	return offset * .01; // Convert to rad / s
}

void gyroUpdateVal(struct task_t *task)
{
	struct gyro_t *g = (struct gyro_t*)task->data;
	g->val = ((g->val * (1.0f - CFG_GYRO_FILTER_ALPHA)) + (sensorGetAdcVal(&g->sensor) * CFG_GYRO_FILTER_ALPHA));
}
