/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "sysinit.h"
#include "esc.h"
#include "logging.h"
#include "proto.h"
#include "state.h"
#include "control.h"
#include "core/uart.h"
#include "core/systick.h"
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
	state_init();

	// Main loop
	while(1) {
		proto_update();
		state_update();
		control_update();
	}

	return 0;
}
