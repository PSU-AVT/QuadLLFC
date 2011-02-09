/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2010, Gregory Haynes
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

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#include <stdint.h>

#include "cpu/cpu.h"
#include "systick/systick.h"
#include "pwm/pwm16.h"
#include "esc/esc.h"
#include "uart/uart.h"
#include "timer/timer32.h"
#include "adc/adc.h"

extern volatile uint32_t timer32_0_counter; // In timer32.c

#define ESC_STARTUP_CYCLE 35000

void setupEscs(void)
{
	struct esc_controller_t *controller;

	controller = escGetController();
	ESC_SETUP(controller->escs[0],
	          ESC_0_PWM_PIN,
	          ESC_0_PWM_TIMER,
	          0)
	ESC_SETUP(controller->escs[1],
	          ESC_1_PWM_PIN,
	          ESC_1_PWM_TIMER,
	          0)
	ESC_SETUP(controller->escs[2],
	          ESC_2_PWM_PIN,
	          ESC_2_PWM_TIMER,
	          0)
	ESC_SETUP(controller->escs[3],
	          ESC_3_PWM_PIN,
	          ESC_3_PWM_TIMER,
	          0)
	escsInit();
	escsArm();
}

uint16_t median3(uint16_t *arr)
{
	uint8_t max, min, i;
	max = 0;
	for(i = 1;i < 3;i++)
	{
		if(arr[i] > arr[max])
			max = i;
	}

	min = 0;
	for(i =1;i < 3;i++)
	{
		if(arr[i] < arr[min])
			min = 1;
	}

	switch(min + max)
	{
	case 0:
	case 1:
		return arr[2];
	case 2:
		return arr[1];
	case 3:
	default:
		return arr[0];
	}
}

int main(void)
{
	struct esc_controller_t *controller;

	cpuInit();
	systickInit(1);

	// Initialize ADC
	adcInit(ADC_PIN0 | ADC_PIN1 | ADC_PIN2 | ADC_PIN3 |  ADC_PIN5 | ADC_PIN6);
	adcSelectPins(ADC_PIN0 | ADC_PIN1 | ADC_PIN2 | ADC_PIN3 |  ADC_PIN5 | ADC_PIN6);
	adcStart();

	proto_init();

	// Arm ESCs
	uartSend("Arming\n", 7);
	setupEscs();
	uartSend("Armed\n", 6);

	uint32_t last_acc_update;
	uint32_t last_gyro_update;

	uint16_t gyro_vals[3];
	uint16_t accel_vals[3];
	uint16_t motors[4];

	uint16_t gyro_bias[3];

	while(1)
	{
		systickDelay(5);


		// Grab gyro vals
		gyro_vals[0] = adcGetNdxVal(0);
		gyro_vals[1] = adcGetNdxVal(1);
		gyro_vals[2] = adcGetNdxVal(2);

		// Grab accelerometer vals
		accel_vals[0] = adcGetNdxVal(3);
		accel_vals[1] = adcGetNdxVal(5);
		accel_vals[2] = adcGetNdxVal(6);


	}

	controller = escGetController();

	return 0;
}
