/*
 * HMC5883L
 *
 * Author: Eric Dinger, Jen Hanni
 * Heavily depended on M.Ryan (ryanmechantronics.com)'s example itg3200.h
 * Created on: 1/2012
 */

#ifndef _HMC5883L_
#define _HMC5883L_

#include "../lpc134x.h"
#include "../core/i2cError.h"
#include "../core/i2c.h"

#define HMC5883L_ADDRESS (0x3C) //0x1E in the datasheet shifted.
#define HMC5883L_READBIT (0x01)

//Device registers - given in the data sheet

#define HMC_5883L_CONFIG_A 0x00
#define HMC_5883L_CONFIG_B 0x01
#define HMC_5883L_MODE     0x02
#define HMC_5883L_DATAX0   0x03
#define HMC_5883L_DATAX1   0x04
#define HMC_5883L_DATAZ0   0x05
#define HMC_5883L_DATAZ1   0x06
#define HMC_5883L_DATAY0   0x07
#define HMC_5883L_DATAY1   0x08
#define HMC_5883L_STATUS   0x09
#define HMC_5883L_ID_A     0x0A
#define HMC_5883L_ID_B     0x0B
#define HMC_5883L_ID_C     0x0C

#define HMC_5883L_MODE_CFG_CONT 0x00

typedef enum
{
  hmc5883_ERROR_OK = 0,		// Everything executed normally
  hmc5883_ERROR_I2CINIT,	// Unable to initialize I2C
  hmc5883_ERROR_I2CBUSY,	// I2C already in use
  hmc5883_ERROR_LAST
} hmc5883lError_e;

#pragma pack (push) // what
#pragma pack (1)    // is this

// isn't the above the same as?
#pragma pack (push, 1)

typedef struct
{
  short status; // whoami
} MagData;

#endif
