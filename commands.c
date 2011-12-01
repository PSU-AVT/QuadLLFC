/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "commands.h"
#include "esc.h"
#include "setpoint.h"
#include "control.h"
#include "state.h"
#include "core/uart.h"
#include "afproto.h"
#include "logging.h"

#include <string.h>

void commands_set_motor(unsigned char *buff, uint8_t length);

//This is for debugging purposes - should not be used!
typedef union throt_data {
	int i;
	float f;
} throt_data;

//Despite the name command_send, can also be used to send messages to HLFC
void command_send(command_out_id id, const unsigned char *data, uint16_t len) {
	unsigned char msg_buff[256];
	unsigned char out_buff[256];

	if(len > 128) {
		logging_send_string(LOGGING_ERROR, "Attempting to send string over 128 chars");
		return;
	}

	strncpy((char*)msg_buff, (char*)data, len);
	afproto_serialize_payload(msg_buff, len+1, out_buff);
	uartSend(out_buff, len+1);
}

void commands_handle_message(unsigned char *buff, uint8_t length) {
	uint8_t ndx = buff[0];
	state_t *current_state = setpoint_get();
	float val;
	float motor_vals[ESC_CNT];
	int i;

	switch(ndx) 
	{
		case 1: //increase roll setpoint
			val = *((float *)&buff[1]);
			current_state->roll = val;
			break;
		case 2: //Increase pitch setpoint
			val = *((float *)&buff[1]);
			current_state->pitch = val;
			break;
		case 0xD3: //Set  motor speed. This is demo/debug purpose ONLY!
			//This will be a floating point from 0 - 1
			val = *((float *)&(buff[1]));
			for (i = 0; i < ESC_CNT; i++) {
				motor_vals[i] = val;
			}
			esc_set_all_throttles(motor_vals); //This is for testing ONLY!
			break;
		case 4: //Set motor speed (state monitored)
                        val = *((float *)&buff[1]);
			current_state->z = val;
			break;
		case 5: //Set yaw setpoint
			val = *((float *)&buff[1]);
			current_state->yaw = val;
			break;
		case 6: // shutdown
			control_set_enabled(0);
			for(i = 0;i < ESC_CNT;++i)
				motor_vals[i] = 0;
			esc_set_all_throttles(motor_vals);
			break;
		case 7: // turn on
			state_reset();
			control_set_enabled(1);
			break;
		case 8: // Set P gains
			if(length == sizeof(state_t)+1)
				control_set_p_gains((state_t*)(&buff[1]));
			break;
		case 9: // Set I gains
			if(length == sizeof(state_t)+1)
				control_set_i_gains((state_t*)(&buff[1]));
			break;
		case 10: // Set D gains
			if(length == sizeof(state_t)+1)
				control_set_d_gains((state_t*)(&buff[1]));
			break;
		default:
			//Should send back some sort of error message here...
			logging_send_string(LOGGING_ERROR, "Received invalid command id");
                        logging_send_buff(LOGGING_ERROR, buff[0], 1);
			break;
	}
}

