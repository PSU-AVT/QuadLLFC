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

void movement_y(float value)
{
	motorsThrustIncreaseAll(-value);
}
