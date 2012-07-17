/**************************************************************************/
/*!
    @file     itg3200.h
    @author   M.Ryan (ryanmechatronics.com)
    @date     8 Aug 2010
    @version  0.10

    @section LICENSE

    Software License Agreement

    Copyright (c) 2010, Ryan Mechatronics LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#ifndef _itg3200_H_
#define _itg3200_H_

//#include "../config.h"
#include "../lpc134x.h"
#include "../core/i2c.h"
#include "../core/i2cError.h"

//0.06956522 for degrees
//0.00121414 for radians
#define LSB_CORRECTION 823.626831

#define itg3200_ADDRESS (0xD0) // device ID was 0xD0
#define itg3200_READBIT (0x01)

#define itg3200_REGISTER_CONFIG_SMPLDIV (0x15) // Sample rate divider - 1
#define itg3200_REGISTER_CONFIG_DLPF (0x16) //Config byte required - must set bits 4 and 5 (i.e. 0x18)
#define itg3200_REGISTER_CONFIG_PMU  (0x3E) //Config byte required - Power Mgmt.  Switch oscillator to gyro for stability (i.e. 0x01)


#define itg3200_REGISTER_WHOAMI (0x00)


#define itg3200_REGISTER_TEMPH  (0x1B)
#define itg3200_REGISTER_TEMPL  (0x1C)
//16 bit two's complement outputs
#define itg3200_REGISTER_XoutH  (0x1D)
#define itg3200_REGISTER_XoutL  (0x1E)
#define itg3200_REGISTER_YoutH  (0x1F)
#define itg3200_REGISTER_YoutL  (0x20)
#define itg3200_REGISTER_ZoutH  (0x21)
#define itg3200_REGISTER_ZoutL  (0x22)


typedef enum
{
  itg3200_ERROR_OK = 0,               // Everything executed normally
  itg3200_ERROR_I2CINIT,              // Unable to initialise I2C
  itg3200_ERROR_I2CBUSY,              // I2C already in use
  itg3200_ERROR_LAST
}
itg3200Error_e;


#pragma pack (push)
#pragma pack (1)
typedef struct
{
        short status; //low byte is "whoami".  High is pending
        short raw_temp;
        short raw_X; // 
        short raw_Y;
        short raw_Z; 
        float temp; // convert to our desired g range
        float X; 
        float Y;
        float Z;
        float x_bias;
        float y_bias;
        float z_bias;
}
GyroData;  // look up attribute flags

i2c_error itg3200Init(void);
i2c_error itg3200GetData (GyroData *data);
unsigned char itg3200_Get_WhoAmI (void);
void itg3200Calibrate(GyroData *data, uint32_t cnt, uint32_t delay);

#endif
