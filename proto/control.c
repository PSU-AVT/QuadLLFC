#include "control.h"
#include "proto.h"
#include "../control/response.h"

void control_handle_msg(const unsigned char *buffer, uint8_t length) {
	uint8_t id = (uint8_t)buffer[0];
	switch(id) {
		case 1: // increase roll setpoint
			if(length > 1) {
				float val = (int8_t)buffer[1];
				response_controller_get()->state_setpoint[AxisRoll] = -val * 0.00154;
				proto_frame_and_send_string("got increase roll");
			} else {
				proto_frame_and_send_string("invalid length for increase roll");
			}
			break;
		case 2: // increase pitch setpoint
			if(length > 1) {
				float val = (int8_t)buffer[1];
				response_controller_get()->state_setpoint[AxisPitch] = val * 0.00154;
				proto_frame_and_send_string("got increase pitch");
			} else {
				proto_frame_and_send_string("invalid length for increase pitch");
			}
			break;
		case 3: // increase vertical setpoint
			if(length > 1) {
				float val = (uint8_t)buffer[1];
				response_controller_get()->state_setpoint[AxisY] = val / 2.6;
				proto_frame_and_send_string("got increase vertical");
			} else {
				proto_frame_and_send_string("invalid length for increase vertical");
			}
			break;
		case 4: // Pong
			break;
	}
}
