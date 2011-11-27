#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef enum command_out_id {
	COMMAND_DEBUG = 2,
	COMMAND_ERROR = 3,
	COMMAND_GYRO_STATE = 4,
	COMMAND_ACCELERO_STATE = 5,
	COMMAND_INERTIAL_STATE = 6,
	COMMAND_MOTORS_STATE = 7,
} command_out_id;

/* Send command using buff to generate data. buff == data is valid */
void command_send(command_out_id id, const unsigned char *data, uint16_t len);

/* Called after receiving an afproto frame */
void commands_handle_message(unsigned char *buff, uint8_t lenth);

#endif
