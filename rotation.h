/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef ROTATION_H
#define ROTATION_H

void rotation_matrix_normalize(float r[][3]);
void rotation_matrix_get_eulers(float r[][3], float eulers[3]);
void rotation_matrix_velocity_update(float r[][3], float roll, float pitch, float yaw, float dt);

#endif
