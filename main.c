/*
 * Author: Gregory Haynes 	<greg@greghaynes.net>
 *         Jonathan Harker 	<kjharke@cs.pdx.edu>
 *         Spencer Krum		<krum.spencer@gmail.com>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2010-2011, Gregory Haynes, Spencer Krum
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

#include <stdint.h>

#include "cpu/cpu.h"
#include "uart/uart.h"
#include "systick/systick.h"
#include "control/motor.h"
#include "control/state.h"
#include "control/response.h"
#include "proto/proto.h"
#include "utils/string.h"

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
	case 'o': // Turn on
		response_off();
		stateReset();
		response_reset();
		response_on();
		break;
	case 'd':
		response_controller_get()->state_setpoint[AxisRoll] += .03;
		break;
	case 'a':
		response_controller_get()->state_setpoint[AxisRoll] -= .03;
		break;
	case 's':
		response_controller_get()->state_setpoint[AxisPitch] += .03;
		break;
	case 'w':
		response_controller_get()->state_setpoint[AxisPitch] -= .03;
		break;
	}
	char out[5];
	strcpy(out, "Got ");
	out[4] = ch;
	proto_frame_and_send(out, 5);
}

int main(void)
{
	cpuInit();
	systickInit(1);
	uartInit(38400);

	while(1) {
		systickDelay(500);
		proto_frame_and_send_string("Hello");
	}

	proto_frame_and_send_string("Leeeerooooyyyyy");

	// Initialize motor pins
	motorsInit();

	// Initialize state system
	stateInit();

	// Start motors
	motorsStart();

	// Start state recording
	stateStart();

	// Set PID Gains
	response_set_p_gain(AxisY, 0.1);
	response_set_p_gain(AxisRoll, 1.2);
	response_set_p_gain(AxisPitch, 1.0);
	response_set_p_gain(AxisYaw, 1.0);

	response_set_d_gain(AxisRoll, 0.28);
	response_set_d_gain(AxisPitch, 0.25);
	response_set_d_gain(AxisYaw, 0.5);

	response_set_i_gain(AxisRoll, 0.0007);
	response_set_i_gain(AxisPitch, 0.0007);
	response_set_i_gain(AxisYaw, 0.0001);

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
