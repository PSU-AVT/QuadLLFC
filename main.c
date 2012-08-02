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
#include "sensors/hmc5883l.h"

int main(void) {
	systemInit();
	// Initialize Systick to be 1ms
	systickInit(1);
        control_init();

	// Uart is 115200 Baud, 8 N 1
	uartInit(57600);

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
	logging_send_string(LOGGING_DEBUG, "Initializing magnetometer.");
	hmc5883lInit();
	logging_send_string(LOGGING_DEBUG, "Initializing magnetometer complete.");

	state_init();
	control_init();

//        state_t inert_test;
//        MagData mag_test;

	logging_send_string(LOGGING_DEBUG, "Entering main control loop.");
	// Main loop
	while(1) {
//                hmc5883lGetData(&mag_test);
//                inert_test.roll = mag_test.X;
//                inert_test.pitch = mag_test.Y;
//                inert_test.yaw = mag_test.Z;
//                inert_test.yaw = atan2(mag_test.Y,mag_test.X);
//                command_send(COMMAND_INERTIAL_STATE, (unsigned char*)&inert_test, sizeof(state_t));
		proto_update();
		state_update();
		control_update();
		esc_update();
	}

	return 0;
}
