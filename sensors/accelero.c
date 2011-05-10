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
#include "../config.h"
#include "../adc/adc.h"

#include <math.h>

void acceleroUpdate(struct task_t *task)
{
	struct accelero_t *a;
	a = (struct accelero_t*)task->data;
	a->val = ((a->val * (1.0f - CFG_ACCELERO_FILTER_ALPHA)) + (sensorGetAdcVal(&a->sensor) * CFG_ACCELERO_FILTER_ALPHA));
}

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

	a->x_update_task.data = &a->x;
	a->y_update_task.data = &a->y;
	a->z_update_task.data = &a->z;

	a->x_update_task.msecs = CFG_ACCELERO_FILTER_MSECS;
	a->y_update_task.msecs = CFG_ACCELERO_FILTER_MSECS;
	a->z_update_task.msecs = CFG_ACCELERO_FILTER_MSECS;

	a->x_update_task.handler = acceleroUpdate;
	a->y_update_task.handler = acceleroUpdate;
	a->z_update_task.handler = acceleroUpdate;
}

void accelero3dStart(struct accelero3d_t *a)
{
	acceleroStart(&a->x);
	acceleroStart(&a->y);
	acceleroStart(&a->z);

	a->x.base_val = CFG_ACCELERO_X_BASE_VAL;
	a->y.base_val = CFG_ACCELERO_Y_BASE_VAL;
	a->z.base_val = CFG_ACCELERO_Z_BASE_VAL;

	a->x.val = a->x.val;
	a->y.val = a->y.val;
	a->z.val = a->z.val;

	a->x.val_scale_acc_div_g = (1.0f / 101);
	a->y.val_scale_acc_div_g = (1.0f / 104);

	tasks_add_task(&a->x_update_task);
	tasks_add_task(&a->y_update_task);
	tasks_add_task(&a->z_update_task);
}

float accelero3dGetRoll(struct accelero3d_t *a)
{
	float ret = (a->x.val - a->x.base_val) * a->x.val_scale_acc_div_g;
	if(ret > 1)
		ret = 1;
	else if(ret < -1)
		ret = -1;
	return asin(ret);
}

float accelero3dGetPitch(struct accelero3d_t *a)
{
	float ret = (a->y.val - a->y.base_val) * a->x.val_scale_acc_div_g;
	if(ret > 1)
		ret = 1;
	else if(ret < -1)
		ret = -1;
	return asin(ret);
}
