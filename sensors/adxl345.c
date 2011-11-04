/*
 * adxl345.c
 *
 *  Created on: Oct 5, 2011
 *      Author: Eric Dinger
 */

#include "adxl345.h"
#include <math.h>


extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

static uint32_t adxl345WriteByte (uint8_t address, uint8_t reg, uint32_t value)
{
  // Clear write buffers
  uint32_t i;
  for (i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = address;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  I2CMasterBuffer[2] = (value & 0xFF);            // Value to write
  return i2cEngine();
}

static uint32_t adxl345ReadByte(uint8_t address, uint8_t reg, int *value)
{
  // Clear write buffers
  uint32_t i;
  for (i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 0;
  I2CReadLength = 1;
  I2CMasterBuffer[0] = address;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = address | adxl345_READBIT;
  uint32_t returned = i2cEngine();
  if(returned)
  {
	  // Shift values to create properly formed integer
	  *value = I2CSlaveBuffer[0];
  }

  return returned;
}

static uint32_t adxl345ReadMultiByte(uint8_t address, uint8_t reg, uint8_t numBytes, uint8_t value[])
{
  // Clear write buffers
  uint32_t i;
  for (i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 0;
  I2CReadLength = numBytes;
  I2CMasterBuffer[0] = address;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = address | adxl345_READBIT;
  uint32_t returned = i2cEngine();
  if(returned)
  {
	  int i;
	  for(i = 0; i < numBytes; i++)
	  {
		  value[i] = I2CSlaveBuffer[i];
	  }
  }

  return returned;
}

i2c_error adxl345_Init(void)
{
	int checkValue;
	i2c_error response;
	// Initialise I2C
	if (i2cInit(I2CMODE_MASTER) == FALSE) {
		return false; /* Fatal error */
	}

	response = adxl345WriteByte(adxl345_ADDRESS, adxl345_REGISTER_POWER_CTL, adxl345_REGISTER_CONFIG_POWER_CTL);
	response = adxl345ReadByte(adxl345_ADDRESS, adxl345_REGISTER_POWER_CTL, &checkValue);

	if ((response == i2c_ok) && (checkValue == adxl345_REGISTER_CONFIG_POWER_CTL))
		accel_init = 1;
	else
		accel_init = 0;

	return response;
}


i2c_error adxl345GetData(AccelData *data)
{
	uint8_t tempData[6];
	i2c_error response = i2c_error_last;
	//int tByte;

	//Check if the Accel has been initialzed
	if (!accel_init)
		adxl345_Init();

	//Doing a multibyte read like the data sheet suggests
	response = adxl345ReadMultiByte(adxl345_ADDRESS, adxl345_REGISTER_DATAX0, 6, tempData);

	//Data comes over in two 8 bit chunks, assembling them into one short.
	data->raw_x = (short) ((int)tempData[1] << 8) | ((int) tempData[0]);
	data->raw_y = (short) ((int)tempData[3] << 8) | ((int) tempData[2]);
	data->raw_z = (short) ((int)tempData[5] << 8) | ((int) tempData[4]);

	//Getting the value in G's lsb correction is defined in adxl345.h
	data->x = data->raw_x * adxl345_LSB_CORRECTION;
	data->y = data->raw_y * adxl345_LSB_CORRECTION;
	data->z = data->raw_z * adxl345_LSB_CORRECTION;

	//Getting the values in euler angles. These are taking directly from freescale's application note AN3416
	data->roll = atan((data->x / sqrt((data->y*data->y)+(data->z*data->z)))); //Labeled x on the sensor stick
	data->pitch = atan((data->y / sqrt((data->x*data->x)+(data->z*data->z)))); //Labeled y on the sensor stick



	return response;
}

