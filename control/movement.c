#include "movement.h"

void movement_roll(float value)
{
	motorNdxThrustIncrease(MOTOR_LF, value);
	motorNdxThrustIncrease(MOTOR_RR, -value);
}

void movement_pitch(float value)
{
	motorNdxThrustIncrease(MOTOR_RF, value);
	motorNdxThrustIncrease(MOTOR_LR, -value);
}

void movement_yaw(float value)
{
	motorNdxThrustIncrease(MOTOR_RF, value);
	motorNdxThrustIncrease(MOTOR_LR, value);
	motorNdxThrustIncrease(MOTOR_LF, -value);
	motorNdxThrustIncrease(MOTOR_RR, -value);
}

void movement_x(float value)
{
}

void movement_y(float value)
{
	motorsThrustIncreaseAll(-value);
}

void movement_z(float value)
{
}

void (*movement_handlers[6])(float) = {
		movement_roll,
		movement_pitch,
		movement_yaw,
		movement_x,
		movement_y,
		movement_z
};

void movement(int type, float value)
{
	movement_handlers[type](value);
}
