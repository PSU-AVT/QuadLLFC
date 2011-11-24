/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "commands.h"
#include "esc.h"
#include "setpoint.h"

#define COMMAND_HANDLER_MAX 0

void commands_set_motor(unsigned char *buff, uint8_t length);

//This is for debugging purposes - should not be used!
typedef union throt_data {
	int i;
	float f;
} throt_data;

void commands_handle_message(unsigned char *buff, uint8_t length) {
	uint8_t ndx = buff[0];
	state_t current_state;// = setpoint_get();
	float val;
	//if(ndx > COMMAND_HANDLER_MAX)
	//	return;
	//command_handlers[ndx](buff, length);
	switch(ndx) 
	{
		case 1: //increase roll setpoint
			val = *((float *)&buff[1]);
			current_state.roll = val;
			break;
		case 2: //Increase pitch setpoint
			val = *((float *)&buff[1]);
			current_state.pitch = val;
			break;
		case 3: //Set  motor speed. This is demo/debug purpose ONLY!
			//This will be a floating point from 0 - 1
			val = *((float *)&(buff[1]));
			float temp[ESC_CNT];
			int i;
			for (i = 0; i < ESC_CNT; i++) {
				temp[i] = val;
			}
			esc_set_all_throttles(temp); //This is for testing ONLY!
			break;
		case 4: //Increase altitude setpoint (state monitored altitude)
			//Not yet implemented...
			break;
		case 5: //Set yaw setpoint
			val = *((float *)&buff[1]);
			current_state.yaw = val;
			break;
		default:
			//Should send back some sort of error message here...
			break;
	}
}

void commands_set_motor(unsigned char *buff, uint8_t length) {
}

