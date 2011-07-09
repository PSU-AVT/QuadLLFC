#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "motor.h"

void movement_roll(float value);
void movement_pitch(float value);
void movement_yaw(float value);
void movement_y(float value);

void movement(int type, float value);

#endif

