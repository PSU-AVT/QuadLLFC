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
#include "adc/adc.h"
#include "sensors/gyro.h"
#include "sensors/accelero.h"
#include "kalman/kalman.h"

extern volatile uint32_t timer32_0_counter; // In timer32.c

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

int main(void)
{
	struct esc_controller_t *controller;

	cpuInit();
	systickInit(1);

	// Arm ESCs
	uartSend("Arming\n", 7);
	setupEscs();
	uartSend("Armed\n", 6);

	// gyro init
	struct gyro3d_t gyros;
	gyro3dInit(&gyros, ADC_PIN0, ADC_PIN1, ADC_PIN2);

	// Accelerometer init
	struct accelero3d_t accelero;
	accelero3dInit(&accelero, ADC_PIN3, ADC_PIN5, ADC_PIN6);

	// Kalman filters init
	struct kalman1d_t k_roll, k_pitch;
	kalman1d_init(&k_roll, 0.0001, 0.0003, 0.69);
	kalman1d_init(&k_pitch, 0.0001, 0.0003, 0.69);

	sensorsStart();
	gyro3dStart(&gyros);
	accelero3dStart(&accelero);

	uint32_t last_predict = systickGetTicks();
	uint32_t last_update = systickGetTicks();
	uint32_t cur_ticks;
	float predict_dt = .001 * CFG_CTL_K_PREDICT;
	float update_dt = .001 * CFG_CTL_K_UPDATE;
	float dt;

	while(1)
	{
		cur_ticks = systickGetTicks();

		// Check for predict timer
		if((dt = (cur_ticks - last_predict)) >= CFG_CTL_K_PREDICT)
		{
			dt *= predict_dt ; // Ticks are in MS
			kalman1d_predict(&k_roll, gyroGetAngVel(&gyros.roll), dt);
			kalman1d_predict(&k_pitch, gyroGetAngVel(&gyros.pitch), dt);
		}

		// Check for update timer
		if((dt = (cur_ticks - last_update)) >= CFG_CTL_K_UPDATE)
		{
			dt *= update_dt; // Ticks are in MS
			kalman1d_update(&k_roll, accelero3dRoll(&accelero));
			kalman1d_update(&k_pitch, accelero3dPitch(&accelero));
		}
	}

	controller = escGetController();

	return 0;
}
