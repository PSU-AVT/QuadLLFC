/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2010, Gregory Haynes, Spencer Krum
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
#include "adc/adc.h"
#include "sensors/gyro.h"
#include "sensors/accelero.h"
#include "kalman/kalman.h"
#include "control/motor.h"

extern volatile uint32_t timer32_0_counter; // In timer32.c

#define DEBUG 1

int main(void)
{
	struct motor_controller_t *mc = motorControllerGet();

	cpuInit();
	systickInit(1);
	uartInit(9600);

	motorsInit();
	motorsStart();

	uint8_t ch;
	char buff[512];
	while(1)
	{
		if(uartRxBufferDataPending())
		{
			ch = uartRxBufferRead();
			if(ch == '.')
			{
#if DEBUG
				sprintf(buff, "%f\r\n", mc->motors[1].duty_cycle);
				uartSend(buff, strlen(buff));
#endif
				motorsThrustIncreaseAll(100);
			}
			else if(ch == '-')
			{
#if DEBUG
				sprintf(buff, "%f\r\n", mc->motors[1].duty_cycle);
				uartSend(buff, strlen(buff));
#endif
				motorsThrustIncreaseAll(-100);
			}

			motorsSyncDutyCycle();
		}
	}

}

#if 0 // Real control loop
int main(void)
{
	struct esc_controller_t *controller;

	cpuInit();
	systickInit(1);
	uartInit(9600);

	motorsInit();

	// gyro init
	struct gyro3d_t gyros;
	gyro3dInit(&gyros, ADC_PIN0, ADC_PIN1, ADC_PIN2);

	// Accelerometer init
	struct accelero3d_t accelero;
	accelero3dInit(&accelero, ADC_PIN3, ADC_PIN5, ADC_PIN7);

	// Kalman filters init
	struct kalman1d_t k_roll, k_pitch;
	kalman1d_init(&k_roll, 0.0001, 0.0003, 0.69);
	kalman1d_init(&k_pitch, 0.0001, 0.0003, 0.69);

	sensorsStart();
	systickDelay(1000); // Let adc settle
	gyro3dStart(&gyros);
	accelero3dStart(&accelero);

	motorsStart();

	uint32_t last_predict = systickGetTicks();
	uint32_t last_update = systickGetTicks();
	uint32_t cur_ticks;
	float predict_dt = .001 * CFG_CTL_K_PREDICT;
	float update_dt = .001 * CFG_CTL_K_UPDATE;
	float dt;
	char buff[150];

	uint8_t ch;

	while(1)
	{
		cur_ticks = systickGetTicks();

		if(uartRxBufferDataPending())
		{
			ch = uartRxBufferRead();
			if(ch == '.')
			{
				uartSend(" --- GOT . ---", strlen(" --- GOT . ---"));
				motorsThrustIncreaseAll(100);
			}
			else if(ch == '-')
			{
				uartSend(" --- GOT - ---", strlen(" --- GOT - ---"));
				motorsThrustIncreaseAll(-100);
			}

			motorsSyncDutyCycle();
		}

		// Check for update timer
		if((dt = (cur_ticks - last_update)) >= CFG_CTL_K_UPDATE)
		{
			float roll = accelero3dGetRoll(&accelero);
			float pitch = accelero3dGetPitch(&accelero);
			float val;
/*
			val = motors[0] + (roll * ROLL_FACTOR);
			if(!(val < MOTOR_MIN || val > MOTOR_MAX))
				motors[0] = val;
			val = motors[1] - (roll * ROLL_FACTOR);
			if(!(val < MOTOR_MIN || val > MOTOR_MAX))
				motors[1] = val;

			val = motors[2] - (pitch * ROLL_FACTOR);
			if(!(val < MOTOR_MIN || val > MOTOR_MAX))
				motors[2] = val;
			val = motors[3] + (pitch * ROLL_FACTOR);
			if(!(val < MOTOR_MIN || val > MOTOR_MAX))
				motors[3] = val;
				*/

			/*
			sprintf(buff, "%f %f %f %f %f\r\n",
					motors[0],
					motors[1],
					motors[2],
					motors[3],
					roll);
			uartSend(buff, strlen(buff));
			*/

			last_update = cur_ticks;
		}
	}

	return 0;
}
#endif
