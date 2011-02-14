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

#include "accelero.h"
#include "../adc/adc.h"

#include <math.h>

void acceleroInit(struct accelero_t *a, uint16_t adc_pin)
{
	sensorInit(&a->sensor, adc_pin);
}

void acceleroStart(struct accelero_t *a)
{
	// Block until we get a valid base accelero value
	while((a->base_val = sensorGetAdcVal(&a->sensor)) == ADC_RESULT_INVALID);
}

void accelero3dInit(struct accelero3d_t *a, uint16_t x_adc_pin,
                    uint16_t y_adc_pin,
                    uint16_t z_adc_pin)
{
	acceleroInit(&a->x, x_adc_pin);
	acceleroInit(&a->y, y_adc_pin);
	acceleroInit(&a->z, z_adc_pin);
}

void accelero3dStart(struct accelero3d_t *a)
{
	//acceleroStart(&a->x);
	//acceleroStart(&a->y);
	//acceleroStart(&a->z);
	a->x.base_val = 444;
	a->y.base_val = 526;
	a->z.base_val = 510;
}

float accelero3dGetRoll(struct accelero3d_t *a)
{
	float y = sensorGetAdcVal(&a->y.sensor);
	y -= a->y.base_val;
	float z = sensorGetAdcVal(&a->z.sensor);
	z -= a->z.base_val;
	z = -z;
	if(!z)
		return 0;
	return atan2f(y, z);
}

float accelero3dGetPitch(struct accelero3d_t *a)
{
	float x = sensorGetAdcVal(&a->x.sensor);
	x -= a->x.base_val;
	float z = sensorGetAdcVal(&a->z.sensor);
	z -= a->z.base_val;
	z = -z;
	if(!z)
		return 0;
	return atan2f(x, z);
}
