/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Gregory Haynes, Spencer Krum
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

#ifndef STATE_H
#define STATE_H

#include "../sys/tasks.h"
#include "../sensors/itg3200.h"

enum axis_t {
	AxisRoll,
	AxisPitch,
	AxisYaw,
	AxisX,
	AxisY,
	AxisZ
};

#define AXIS_CNT 6

struct state_controller_t
{
	GyroData gyros;
    float body_state_delta[AXIS_CNT];
    float body_state_dt[AXIS_CNT];
    float inertial_state[AXIS_CNT];
    float inertial_stat_accum[AXIS_CNT];
    float r_b_to_i[3][3];
};

struct state_controller_t *stateControllerGet(void);

/* Initialize the state controller */
void stateInit(void);
void stateStart(void);

/* Difference of two float arrays of size AXIS_CNT
 * dest[i] = a[i] - b[i]
 */
void stateSubtract(float *a, float *b, float *dest);

void stateCopy(const float *src, float *dst);

void stateAdd(float *a, float *b, float *dest);

void stateScale(float *a, float b);

void stateReset(void);

#endif

