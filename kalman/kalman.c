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

#include "kalman.h"

void kalman1d_init(struct kalman1d_t *k, float q_angle, float q_gyro, float r_angle)
{
	k->q_angle = q_angle;
	k->q_gyro = q_gyro;
	k->r_angle = r_angle;
}

void kalman1d_predict(struct kalman1d_t *k, float gyro, float dt)
{
	k->angle = dt * (gyro - k->bias);
	k->p_00 = - dt * (k->p_10 + k->p_01) + k->q_angle * dt;
	k->p_01 +=  - dt * k->p_11;
	k->p_10 +=  - dt * k->p_11;
	k->p_11 +=  + k->q_gyro * dt;
}

void kalman1d_update(struct kalman1d_t *k, float angle)
{
	const float y = angle - k->angle;

	const float S = k->p_00 + k->r_angle;
	const float K_0 = k->p_00 / S;
	const float K_1 = k->p_10 / S;

	k->angle +=  K_0 * y;
	k->bias  +=  K_1 * y;

	k->p_00 -= K_0 * k->p_00;
	k->p_01 -= K_0 * k->p_01;
	k->p_10 -= K_1 * k->p_00;
	k->p_11 -= K_1 * k->p_01;
}
