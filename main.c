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
	adcInit(ADC_PIN0 | ADC_PIN1);
	adcSelectPins(ADC_PIN0 | ADC_PIN1);
	adcStart();

	proto_init();

	// Arm ESCs
	uartSend("Arming\n", 7);
	setupEscs();
	uartSend("Armed\n", 6);

	controller = escGetController();

	// Get base gyro values
	uint8_t gyro_smooth_cnt = 3;
	uint16_t gyro_vals[2][gyro_smooth_cnt];
	uint16_t gyros_base[2];
	uint16_t gyro_prev_vals[2];
	uint8_t gyro_cnt;

	// Get gyro base vals
	for(gyro_cnt=0;gyro_cnt < gyro_smooth_cnt;gyro_cnt++)
	{
		gyro_vals[0][gyro_cnt] = adcGetVal(ADC_PIN1);
		gyro_vals[1][gyro_cnt] = adcGetVal(ADC_PIN0);
//		systickDelay(20);


	}
	gyros_base[0] = median3(gyro_vals[0]);
	gyros_base[1] = median3(gyro_vals[1]);
	gyro_prev_vals[0] = gyros_base[0];
	gyro_prev_vals[1] = gyros_base[1];

	// Control loop
	float motors[4];
	uint16_t gyro_val;
	int offset;
	float reaction_p_factor = 0.00004;
	float reaction_d_factor = .06;
	float reaction;

	uint16_t motor_base = ESC_STARTUP_CYCLE;
	int16_t mod;
	uint8_t cmd;

	motors[0] = motor_base;
	motors[1] = motor_base;
	motors[2] = motor_base;
	motors[3] = motor_base;

	while(1)
	{
		//systickDelay(1);
		if(uartRxBufferDataPending())
		{
			cmd = uartRxBufferRead();
			if(cmd == '.')
			{
				mod = -400;
				uartSendByte('.');
			}
			else if(cmd == '-')
			{
				mod = 400;
				uartSendByte('-');
			}
			else
				mod = 0;
			motors[0] += mod;
			motors[1] += mod;
			motors[2] += mod;
			motors[3] += mod;
		}

		if(gyro_cnt >= 3)
			gyro_cnt = 0;
		gyro_vals[0][gyro_cnt] = adcGetVal(ADC_PIN1);
		gyro_vals[1][gyro_cnt] = adcGetVal(ADC_PIN0);
		gyro_cnt++;
		gyro_val = median3(gyro_vals[0]);

		// P gyro 1
		if(gyro_val != gyros_base[0])
		{
			offset = gyro_val - gyros_base[0];
			reaction = offset >> 13; // P factor
			reaction = 0;
			motors[0] -= reaction;
			motors[3] += reaction;

		}

		// D gyro 1
		offset = gyro_val - gyro_prev_vals[0];
		reaction = offset >> 3; // D factor
		motors[0] -= reaction;
		motors[3] += reaction;
		gyro_prev_vals[0] = gyro_val;

		// P gyro 2
		gyro_val = median3(gyro_vals[1]);
		if(gyro_val != gyros_base[1])
		{
			offset = gyro_val - gyros_base[1];
			reaction = offset >> 13; // P factor
			reaction = 0;
			motors[1] -= reaction;
			motors[2] += reaction;
		}

		// D gyro 1
		offset = gyro_val - gyro_prev_vals[1];
		reaction = offset >> 3; // D Factor
		motors[1] -= reaction;
		motors[2] += reaction;
		gyro_prev_vals[1] = gyro_val;

		escSetDutyCycle(&(controller->escs[0]),
		                motors[0]);
		escSetDutyCycle(&(controller->escs[1]),
		                motors[1]);
		escSetDutyCycle(&(controller->escs[2]),
		                motors[2]);
		escSetDutyCycle(&(controller->escs[3]),
		                motors[3]);
	}

	return 0;
}
