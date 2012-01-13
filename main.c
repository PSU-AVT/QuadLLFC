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
	// Initialize Systick to be 1ms
	systickInit(1);

	// Uart is 115200 Baud, 8 N 1
	uartInit(115200);

	logging_send_string(LOGGING_DEBUG, "Hello!");

	// Initialize motor controllers
	esc_init_all();
	logging_send_string(LOGGING_DEBUG, "Arming ESCs.");
	esc_arm_all();
	logging_send_string(LOGGING_DEBUG, "Arming ESCs complete.");

	// Init sensors
	logging_send_string(LOGGING_DEBUG, "Initializing gyro.");
	itg3200Init();
	logging_send_string(LOGGING_DEBUG, "Initializing gyro complete.");
	logging_send_string(LOGGING_DEBUG, "Initializing accelerometer.");
	adxl345_Init();
	logging_send_string(LOGGING_DEBUG, "Initializing accelerometer complete.");

	state_init();

	logging_send_string(LOGGING_DEBUG, "Entering main control loop.");
	// Main loop
	while(1) {
		proto_update();
		state_update();
		control_update();
	}

	return 0;
}
