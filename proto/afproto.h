#ifndef AFPROTO_H
#define AFPROTO_H

#include <stdint.h>

#define AFPROTO_FRAME_START_BYTE 0x0d

typedef struct AfprotoFrame {
	uint8_t length;
	uint8_t crc;
	const char *payload;
} AfprotoFrame;

void afproto_get_frame(const char *buffer,
                       uint16_t length,
                       uint8_t *offset,
                       AfprotoFrame *frame);

void afproto_serialize_frame(char *buffer,
                             uint8_t offset,
                             AfprotoFrame *frame);

void afproto_create_frame(const char *buffer,
                          uint8_t length,
                          AfprotoFrame *frame);

#endif
