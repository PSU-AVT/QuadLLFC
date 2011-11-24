/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef CRC_8_H
#define CRC_8_H

unsigned char crc_8_update(unsigned char crc, unsigned char c);

unsigned char crc_8(unsigned const char *pDst, unsigned char length);

#endif
