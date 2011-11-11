/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "esc.h"
#include "logging.h"
#include "core/uart.h"

int main(void) {
	systemInit();
	uartInit(115200);
	esc_init_all();
	esc_arm_all();

	// Main loop
	while(1) {
		uartSendByte('a');
		systickDelay(1000);
	}

	return 0;
}
