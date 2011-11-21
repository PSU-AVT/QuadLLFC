#include "commands.h"

#define COMMAND_HANDLER_MAX 0

typedef void (*command_handler)(unsigned char *buff, uint8_t length);

void commands_set_motor(unsigned char *buff, uint8_t length);

static command_handler command_handlers[] = {
	commands_set_motor,
};

void commands_handle_message(unsigned char *buff, uint8_t length) {
	uint8_t ndx = buff[0];
	if(ndx > COMMAND_HANDLER_MAX)
		return;
	command_handlers[ndx](buff, length);
}

void commands_set_motor(unsigned char *buff, uint8_t length) {
}

