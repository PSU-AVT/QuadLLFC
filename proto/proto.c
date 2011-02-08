/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Gregory Haynes
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

#include "proto.h"
#include "message.h"
#include "../uart/uart.h"
#include "../crypto/crc8.h"

typedef enum PROTO_PARSE_STATE_T
{
	PROTO_PARSE_STATE_READ,
	PROTO_PARSE_STATE_DISCARD
} PROTO_PARSE_STATE_T;

static volatile PROTO_PARSE_STATE_T _proto_parse_state;
static uint8_t _proto_parse_buff_offset;
static uint8_t *_proto_parse_buff;

void proto_reset(void)
{
	_proto_parse_state = PROTO_PARSE_STATE_DISCARD;
	_proto_parse_buff_offset = 0;
	_proto_parse_buff = uartGetPCB()->rxfifo.buf;
}

void proto_init(void)
{
	uartInit(CFG_UART_BAUDRATE);
	proto_reset();
}

void proto_invalid_message(void)
{
	uartSendByte('!');
}

void proto_check_msg(void)
{
	int length;
	uint8_t check_crc;
	uint8_t real_crc;

	length = (int)_proto_parse_buff[1];
	check_crc = _proto_parse_buff[_proto_parse_buff_offset];
	real_crc = crc8(_proto_parse_buff, length-1);
	if(real_crc == check_crc)
		message_handle(&_proto_parse_buff[1], length-2);
	else
		proto_invalid_message();
}

void proto_process(void)
{
	while (_proto_parse_buff_offset < uartGetPCB()->rxfifo.len)
	{
		// Handle next char in parse buffer
		switch(_proto_parse_state)
		{
		case PROTO_PARSE_STATE_READ:
			if(_proto_parse_buff_offset > 1 &&
			   _proto_parse_buff_offset == _proto_parse_buff[1])
			{
				// At the end of message
				proto_check_msg();
				proto_reset();
			}
			_proto_parse_buff_offset++;
			break;
		case PROTO_PARSE_STATE_DISCARD:
			if(_proto_parse_buff[_proto_parse_buff_offset] == PROTO_MSG_HEADER_TAG)
			{
				_proto_parse_state = PROTO_PARSE_STATE_READ;
				_proto_parse_buff_offset++;
			}
			else
				uartRxBufferClearFIFO();
			break;
		}
	}
}
