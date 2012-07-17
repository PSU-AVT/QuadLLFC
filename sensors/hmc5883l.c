/* Jen Hanni
 * Draws heavily on Ryan@Mechatronics and Eric Dinger
 * HMC5883L driver
*/

#include "hmc5883l.h"
#include "../core/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

uint32_t i;
uint8_t mag_init;

static i2c_error hmc5883lWriteByte (uint8_t address, uint8_t reg, uint32_t value)
{
  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
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

static i2c_error hmc5883lReadByte(uint8_t address, uint8_t reg, int *value)
{
  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 0;
  I2CReadLength = 1;
  I2CMasterBuffer[0] = address;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = address | hmc5883l_READBIT;
  i2c_error status = i2cEngine();
  if(status == i2c_ok)
  {
          // Shift values to create properly formed integer
          *value = I2CSlaveBuffer[0];
  }
  return status;
}

static i2c_error hmc5883lReadMultiByte(uint8_t address, uint8_t reg, uint8_t numBytes, uint8_t value[])
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
  I2CMasterBuffer[2] = address | hmc5883l_READBIT;
  i2c_error returned = i2cEngine();
  if(returned == i2c_ok)
  {
          // Shift values to create properly formed integer
          int i;
          for(i = 0; i < numBytes; i++)
          {
                  value[i] = I2CSlaveBuffer[i];
          }
  }

  return returned;
}

/**************************************************************************/
/*!
    @brief  Initialises the I2C block
*/
/**************************************************************************/

i2c_error hmc5883lInit(void)
{
  i2c_error response_error = i2c_ok;
  // Initialise I2C
  if (i2cInit(I2CMODE_MASTER) == FALSE)
  {
    return i2c_init;    /* Fatal error */
  }

  int checkValue;

  response_error = hmc5883lWriteByte(itg3200_ADDRESS, itg3200_REGISTER_CONFIG_DLPF, 0x19);
  response_error = hmc5883lReadByte(itg3200_ADDRESS, itg3200_REGISTER_CONFIG_DLPF, &checkValue);

  if (checkValue != 0x19)
          return i2c_error_last;

  response_error = hmc5883lWriteByte (itg3200_ADDRESS, itg3200_REGISTER_CONFIG_PMU, 0x01);
  response_error = hmc5883lReadByte(itg3200_ADDRESS, itg3200_REGISTER_CONFIG_PMU, &checkValue);

  if (checkValue != 0x01)
          return i2c_error_last;

  response_error = hmc5883lWriteByte (itg3200_ADDRESS, itg3200_REGISTER_CONFIG_SMPLDIV, 0x07);
  response_error = hmc5883lReadByte(itg3200_ADDRESS, itg3200_REGISTER_CONFIG_SMPLDIV, &checkValue);

  if (checkValue != 0x07)
          return i2c_error_last;

  if (response_error == i2c_ok)
  {
          mag_init = 1;
  } else {
          mag_init = 0;
  }
  return response_error;
}

/**************************************************************************/
/*!
    @brief  Reads the mag data set (full)

    @note   This method will read the entire data set from the magnetometer
             and return it in the mag structure.
*/
/**************************************************************************/

i2c_error hmc5883lGetData (MagData *data)
{
  uint8_t tmpdat[8];
  i2c_error error = i2c_error_last;

  if (!mag_init)
     hmc5883lInit();

  error = hmc5883lReadMultiByte(hmc5883l_ADDRESS, hmc588l_REGISTER_
}
