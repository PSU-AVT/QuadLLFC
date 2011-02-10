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

/**
 * Typical gyro usage:
 *  gyro_t gyro;
 *  gyroInit(&gyro, ADC_PIN0);
 *  sensorsStart();
 *  gyroStart(&gyro);
 */

#ifndef GYRO_H
#define GYRO_H

#include "sensor.h"

struct gyro_t
{
	struct sensor_t sensor;
	uint16_t base_val;
};

struct gyro3d_t
{
	struct gyro_t roll,
	              pitch,
	              yaw;
};

void gyroInit(struct gyro_t *gyro, uint16_t adc_pin);

/*
 * call after sensorsStart (or adcStart()) is called
 */
void gyroStart(struct gyro_t *gyro);

void gyro3dInit(struct gyro3d_t *gyro, uint16_t r_adc_pin,
                uint16_t p_adc_pin,
                uint16_t y_adc_pin);

void gyro3dStart(struct gyro3d_t *gyro);

/* Returns last measured angular velocity of gyro
 * units are deg / s */
float gyroGetAngVel(struct gyro_t *gyro);

#endif
