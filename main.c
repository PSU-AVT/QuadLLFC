/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "LPC13xx.h"

int main(void) {
	if (SysTick_Config (SystemCoreClock / 10000)) { /* Setup SysTick for 100 usec interrupts */
		while (1); /* Dont continue if error */
	}

	// Main loop
	while(1) {

	}

	return 0 ;
}
