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
#include "../crypto/crc8.h"

enum ProtoMsgGetcharRet // Return val for proto_msg_getchar
{
	PROTO_MSG_OK,
	PROTO_MSG_IO_ERROR,
	PROTO_MSG_INVALID,
	PROTO_MSG_COMPLETE
};

enum ProtoMsgParseState_t // State machine for msg parser
{
	PROTO_MSG_PARSE_STATE_DISCARD, // Discard until footer tag
	PROTO_MSG_PARSE_STATE_READ, // Read into msg buffer
	PROTO_MSG_PARSE_STATE_COMPLETE // Complete, valid message stored
};

static volatile uint8_t proto_msg_buff[PROTO_MSG_MAXLENGTH];
static volatile uint8_t proto_msg_buff_offset;
static volatile int proto_msg_parse_state;

void proto_msg_parse_init(void)
{
	proto_msg_parse_state = PROTO_MSG_PARSE_STATE_READ;
	proto_msg_buff_offset = 0;
}

/* Validates msg buffer
   Returns 0 on error, 1 on success. */
int proto_msg_validate(void)
{
	uint8_t crc;
	uint8_t len = proto_msg_buff[0];

	// Footer tag check
	if(proto_msg_buff[len-1] != PROTO_MSG_FOOTER_TAG)
		return 0;

	// CRC check
	crc = crc8(proto_msg_buff, (int)len - 2);
	if(crc != proto_msg_buff[len-2])
		return 0;

	return 1;
}


