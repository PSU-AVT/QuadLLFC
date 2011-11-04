/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

#include "esc.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

int main(void) {
	if (SysTick_Config (SystemCoreClock / 10000)) { /* Setup SysTick for 100 usec interrupts */
		while (1); /* Dont continue if error */
	}

	esc_init_all();
	esc_arm_all();

	// Main loop
	while(1) {

	}

	return 0;
}
