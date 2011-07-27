/*
 * Author: K Jonathan Harker <kjharke@cs.pdx.edu>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, K Jonathan Harker
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

#include <math.h>

#include "translate.h"
#include "state.h"
#include "../utils/matrix.h"

void rotation_matrix_normalize(float r[][3]) {
	float error;
	error = r[0][0]*r[0][1] + r[1][0]*r[1][1] + r[2][0]*r[2][1];
	error /= 2; // Distribute evenly between X and Y

	// Reduce error in X col
	r[0][0] -= error * r[0][1];
	r[1][0] -= error * r[1][1];
	r[2][0] -= error * r[2][1];
	// Reduce error in Y col
	r[0][1] -= error * r[0][0];
	r[1][1] -= error * r[1][0];
	r[2][1] -= error * r[2][0];

	// Set Z col to be orthogonal to X and Y
	// Z = Y cross X
	r[0][2] = r[1][0]*r[2][1] - r[2][0]*r[1][1];
	r[1][2] = r[2][0]*r[0][1] - r[0][0]*r[2][1];
	r[2][2] = r[0][0]*r[1][1] - r[1][0]*r[0][1];
}

// Get euler angles (X, Y', Z'') from R
void rotation_matrix_get_eulers(float r[][3], float eulers[3]) {
	eulers[0] = (float)atan2((double)r[2][1], (double)r[2][2]);
	eulers[1] = (float)asin((double)r[2][0]);
	eulers[2] = (float)atan2((double)r[0][1], (double)r[0][0]);
}

void rotation_matrix_update(struct state_controller_t *sc) {
	// Rotation matrix R(t + dt) = R(t)*(R(t) crossprod (d_theta / dt))
	// This comes out to be:
	// R(t + dt) = R(t) * [  1,             -body_yaw_dt,   body_pitch_dt ;
	//                       body_yaw_dt,    1,            -body_roll_dt  ;
	//                      -body_pitch_dt,  body_roll_dt,  1             ]

	float r_dth_dt[3][3];
	// Row 1
	r_dth_dt[0][0] = 1;
	r_dth_dt[0][1] = -sc->body_state_delta[AxisYaw];
	r_dth_dt[0][2] = sc->body_state_delta[AxisPitch];
	// Row 2
	r_dth_dt[1][0] = sc->body_state_delta[AxisYaw];
	r_dth_dt[1][1] = 1;
	r_dth_dt[1][2] = -sc->body_state_delta[AxisRoll];
	// Row 3
	r_dth_dt[2][0] = -sc->body_state_delta[AxisPitch];
	r_dth_dt[2][1] = sc->body_state_delta[AxisRoll];
	r_dth_dt[2][2] = 1;

	float tmp_matrix[3][3];

	matrix_multiply_3x3(sc->r_b_to_i, r_dth_dt, tmp_matrix);
	matrix_copy_3x3(tmp_matrix, sc->r_b_to_i);

	rotation_matrix_normalize(sc->r_b_to_i);
}
