/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "proto.h"
#include "afproto.h"
#include "commands.h"
#include "core/uart.h"

static unsigned char proto_buff[256];
static uint8_t proto_buff_ndx = 0;

void proto_update(void) {
	while(uartRxBufferDataPending()) {
		proto_buff[proto_buff_ndx] = uartRxBufferRead();
		if(proto_buff[proto_buff_ndx] == AFPROTO_FRAME_END_BYTE &&
		  (proto_buff_ndx == 0 ||
		   proto_buff[proto_buff_ndx-1] != AFPROTO_FRAME_ESCAPE_BYTE)) {
			// Message end
			uint8_t len;
			afproto_get_payload(proto_buff, proto_buff_ndx+1, afproto_get_buffer(), &len);
			commands_handle_message(afproto_get_buffer(), len);
			proto_buff_ndx = 0;
		} else
			++proto_buff_ndx;
	}
}

