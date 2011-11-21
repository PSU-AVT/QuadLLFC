/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "sysinit.h"
#include "esc.h"
#include "logging.h"
#include "proto.h"
#include "core/uart.h"
#include "core/systick.h"
#include "core/pwm.h"
#include "sensors/itg3200.h"
#include "sensors/adxl345.h"

int main(void) {
	systemInit();
	systickInit(1);

	uartInit(115200);

	esc_init_all();
	esc_arm_all();

	// Init sensors
	itg3200Init();
	adxl345_Init();

	float throttle = 0;
	int i;

	pwm_16_set_duty_cycle_in_ticks(PWM_PIN_16_0_0, 30000);
	while(1);

	// Main loop
	while(1) {
		if(uartRxBufferDataPending()) {
			switch(uartRxBufferRead()) {
			case 'w':
				throttle = esc_get_throttle(0);
				for(i = 0;i < 3;++i)
					esc_set_throttle(i, throttle+.05);
				break;
			case 's':
				throttle = esc_get_throttle(0);
				for(i = 0;i < 3;++i)
					esc_set_throttle(i, throttle-.05);
				break;
			case 'p':
				for(i = 0;i < 3;++i)
					esc_set_throttle(i, 0);
				break;
			}
		}
	}

	return 0;
}
