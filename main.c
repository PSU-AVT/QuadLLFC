/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#include "cpu/cpu.h"
#include "systick/systick.h"
#include "pwm/pwm32.h"

int main(void)
{
	uint32_t dutycycle;
	cpuInit();
	pwm32Init(PWM32_TIMER0, PWM32_PIN0_0);

	// Start systick with tick every 10ms
	systickInit(10);

	pwm32SetDutyCycleInTicks(0xFF);
	pwm32Start(PWM32_TIMER0);

	while(1)
	{
		for(dutycycle = 0xFF;dutycycle<0xFFFF;dutycycle++)
		{
			systickDelay(1);
			pwm32SetDutyCycleInTicks(dutycycle);
		}
	}
	return 0 ;
}
