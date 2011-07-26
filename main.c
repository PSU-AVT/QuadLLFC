/*
 * Author: Gregory Haynes 	<greg@greghaynes.net>
 *         Jonathan Harker 	<kjharke@cs.pdx.edu>
 *         Spencer Krum		<krum.spencer@gmail.com>
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
#include <string.h>

#include "cpu/cpu.h"
#include "systick/systick.h"
#include "uart/uart.h"
#include "control/motor.h"
#include "control/state.h"
#include "control/response.h"

#define DEBUG 1

void ctl_increment_i_gain(int axis, float value) {
	char buff[256];
	response_set_d_gain(axis, response_get_i_gain(axis)+value);
	sprintf(buff, "Gain (%d) i value now at %f\r\n", axis, response_get_i_gain(axis)+value);
	uartSend(buff, strlen(buff));
}

void ctl_increment_d_gain(int axis, float value) {
	char buff[256];
	response_set_d_gain(axis, response_get_d_gain(axis)+value);
	sprintf(buff, "Gain (%d) d value now at %f\r\n", axis, response_get_d_gain(axis)+value);
	uartSend(buff, strlen(buff));
}

void ctl_increment_p_gain(int axis, float value) {
	char buff[256];
	response_set_p_gain(axis, response_get_p_gain(axis)+value);
	sprintf(buff, "Gain (%d) p value now at %f\r\n", axis, response_get_p_gain(axis)+value);
	uartSend(buff, strlen(buff));
}

void handle_control_input(char ch)
{
	switch(ch)
	{
	case ']': // Increase thrust
		response_controller_get()->state_setpoint[AxisY] += 1;
		break;
	case '[': // Decrease thrust
		response_controller_get()->state_setpoint[AxisY] -= 1;
		break;
	case 'q': // Turn off
		motors_off();
		response_off();
		break;
	case 's': // Turn on
		response_on();
		break;
	case 'y':
		ctl_increment_p_gain(AxisY, .05);
		break;
	case 'h':
		ctl_increment_p_gain(AxisY, -.05);
		break;
	case 'u':
		ctl_increment_p_gain(AxisRoll, .05);
		break;
	case 'j':
		ctl_increment_p_gain(AxisRoll, -.05);
		break;
	case 'i':
		ctl_increment_p_gain(AxisPitch, .05);
		break;
	case 'k':
		ctl_increment_p_gain(AxisPitch, -.05);
		break;
	case 'e':
		ctl_increment_d_gain(AxisRoll, .05);
		break;
	case 'd':
		ctl_increment_d_gain(AxisRoll, -.05);
		break;
	case 'r':
		ctl_increment_d_gain(AxisPitch, .05);
		break;
	case 'f':
		ctl_increment_d_gain(AxisPitch, -.05);
		break;
	case 'z':
		ctl_increment_i_gain(AxisRoll, .05);
		break;
	case 'x':
		ctl_increment_i_gain(AxisRoll, -.05);
		break;
	case 'c':
		ctl_increment_i_gain(AxisPitch, .05);
		break;
	case 'v':
		ctl_increment_i_gain(AxisPitch, -.05);
		break;
	}
	uartSend("Got ", 4);
	uartSendByte(ch);
	uartSend("\r\n", 2);
}

int main(void)
{
	cpuInit();
	systickInit(1);
	uartInit(38400);

	uartSend("\r\n\r\n", 4);
	uartSend("Leeeeroooooooy....\r\n\r\n", 22);

	// Initialize motor pins
	motorsInit();

	// Initialize state system
	stateInit();

	// Start motors
	motorsStart();

	// Start state recording
	stateStart();

	// Set initial gains
	response_set_p_gain(AxisY, 0.2);
	response_set_p_gain(AxisRoll, 0.3);
	response_set_p_gain(AxisPitch, 0.3);
	response_set_d_gain(AxisRoll, .5);
	response_set_d_gain(AxisPitch, .5);

	// Start the control system
	response_start();

	motors_off();
	response_off();

	while(1)
	{
		while(uartRxBufferDataPending())
		{
			handle_control_input(uartRxBufferRead());
		}
		tasks_loop();
	}

	return 0;
}
