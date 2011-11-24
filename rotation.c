/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "rotation.h"
#include "matrix.h"

#include <math.h>

void rotation_matrix_init(float r[][3]) {
	int i, j;
	for(i = 0;i < 3;++i) {
		for(j = 0;j < 3;++j) {
			if(i == j) r[i][i] = 1;
			else r[i][j] = 0;
		}
	}
}

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

void rotation_matrix_velocity_update(float r[][3], float roll, float pitch, float yaw, float dt) {
	float r_dth_dt[3][3];
	yaw = yaw*dt;
	pitch = pitch*dt;
	roll = roll*dt;

	// Row 1
	r_dth_dt[0][0] = 1;
	r_dth_dt[0][1] = -yaw;
	r_dth_dt[0][2] = pitch;
	// Row 2
	r_dth_dt[1][0] = yaw;
	r_dth_dt[1][1] = 1;
	r_dth_dt[1][2] = -roll;
	// Row 3
	r_dth_dt[2][0] = -pitch;
	r_dth_dt[2][1] = roll;
	r_dth_dt[2][2] = 1;

	float tmp_matrix[3][3];

	matrix_3_3_multiply(r, r_dth_dt, tmp_matrix);
	matrix_3_3_copy(tmp_matrix, r);

	rotation_matrix_normalize(r);
}

