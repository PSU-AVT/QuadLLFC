/*
 * HMC5883L Magnetomer Driver
 *
 * Author: Eric Dinger, Jen Hanni
 * Heavily depended on M.Ryan (ryanmechantronics.com)'s example itg3200.h
 */

#ifndef _HMC5883L_
#define _HMC5883L_

#include "../lpc134x.h"
#include "../core/i2cError.h"
#include "../core/i2c.h"

#define HMC5883L_ADDRESS  (0x3C) 
#define HMC5883L_READBIT  (0x3D)
#define HMC5883L_WRITEBIT (0x3C) 

//Device registers - given in the data sheet

#define HMC5883L_REG_CONFIG_A (0x00)
#define HMC5883L_REG_CONFIG_B (0x01)
#define HMC5883L_REG_MODE     (0x02)
#define HMC5883L_REG_DATAX0   (0x03)
#define HMC5883L_REG_DATAX1   (0x04)
#define HMC5883L_REG_DATAZ0   (0x05)
#define HMC5883L_REG_DATAZ1   (0x06)
#define HMC5883L_REG_DATAY0   (0x07)
#define HMC5883L_REG_DATAY1   (0x08)
#define HMC5883L_REG_STATUS   (0x09)
#define HMC5883L_REG_ID_A     (0x0A)
#define HMC5883L_REG_ID_B     (0x0B)
#define HMC5883L_REG_ID_C     (0x0C)

#define HMC_5883L_MODE_CFG_CONT (0x00)

typedef enum
{
  hmc5883_ERROR_OK = 0,		// Everything executed normally
  hmc5883_ERROR_I2CINIT,	// Unable to initialize I2C
  hmc5883_ERROR_I2CBUSY,	// I2C already in use
  hmc5883_ERROR_LAST
} hmc5883lError_e;

#pragma pack (push) // what
#pragma pack (1)    // is this

typedef struct
{
  short status; // whoami
  int16_t raw_X;
  int16_t raw_Y;
  int16_t raw_Z;
  int16_t X_bias;
  int16_t Y_bias;
  int16_t Z_bias;
  float X;
  float Y;
  float Z;
  uint32_t ID;
} MagData;

i2c_error hmc5883lInit();
i2c_error hmc5883lGetData (MagData *data);
void hmc5883lCalibrate(MagData *data, uint32_t cnt, uint32_t delay);



#endif
