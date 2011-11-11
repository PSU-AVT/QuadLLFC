/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "sysinit.h"
#include "esc.h"
#include "logging.h"
#include "core/uart.h"
#include "core/systick.h"

int main(void) {
	systemInit();

	uartInit(115200);

	esc_init_all();
	esc_arm_all();

	// Main loop
	while(1) {
		logging_send_string(LOGGING_DEBUG, "a");
		systickDelay(1000);
	}

	return 0;
}
