/* Jen Hanni
 * Draws heavily on Ryan@Mechatronics and Eric Dinger itg3200 and adxl345 drivers
 *
 * HMC5883L driver
 * no guarantees.
*/

#include "hmc5883l.h"
#include "../core/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

uint32_t i;
uint8_t mag_init;

/**************************************************************************/
/*!
    @brief  Functions read a byte, write a byte, and read multibytes
*/
/**************************************************************************/

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
  I2CMasterBuffer[2] = address | HMC5883L_READBIT;
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
  I2CMasterBuffer[2] = address | HMC5883L_READBIT;
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

  response_error = hmc5883lWriteByte(HMC5883L_ADDRESS, HMC5883L_REG_CONFIG_A, 0x10);
  response_error = hmc5883lReadByte(HMC5883L_ADDRESS, HMC5883L_REG_CONFIG_A, &checkValue);

  if (checkValue != 0x10)
          return i2c_error_last;

  response_error = hmc5883lWriteByte (HMC5883L_ADDRESS, HMC5883L_REG_CONFIG_B, 0x20);
  response_error = hmc5883lReadByte(HMC5883L_ADDRESS, HMC5883L_REG_CONFIG_B, &checkValue);

  if (checkValue != 0x20)
          return i2c_error_last;

  // default is single measurement mode
  response_error = hmc5883lWriteByte (HMC5883L_ADDRESS, HMC5883L_REG_MODE, 0x00);
  response_error = hmc5883lReadByte(HMC5883L_ADDRESS, HMC5883L_REG_MODE, &checkValue);

  if (checkValue != 0x01)
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

    @caution This method currently returns the raw data
*/
/**************************************************************************/

i2c_error hmc5883lGetData (MagData *data)
{
  uint8_t tmpdat[8];
  i2c_error error = i2c_error_last;

  if (!mag_init)
     hmc5883lInit();

 // not entirely sure about error handling here
  error = hmc5883lReadMultiByte(HMC5883L_ADDRESS, HMC5883L_REG_DATAX0, 8, tmpdat);
  if (error != i2c_ok)
     return error;

  data->raw_X = (short) ((int) tmpdat[0] << 8) | ((int) tmpdat[1]);
  data->raw_Y = (short) ((int) tmpdat[2] << 8) | ((int) tmpdat[3]);
  data->raw_Z = (short) ((int) tmpdat[4] << 8) | ((int) tmpdat[5]);

  // Mag scaling: no idea
  data->X = data->raw_X;
  data->Y = data->raw_Y;
  data->Z = data->raw_Z;

  return error;  
}

/**************************************************************************/
/*!
    @brief  Reads identification registers A, B, C 
            but why and what for?
*/
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Calibrates the magnetometer to determine initial bias
*/
/**************************************************************************/

void hmc5883lCalibrate(MagData *data, uint32_t cnt, uint32_t delay) {
  int i;
  float x, y, z;

  data->X_bias = 0;
  data->Y_bias = 0;
  data->Z_bias = 0;
 
  for(i = 0; i < cnt; i++) {
    hmc5883lGetData(data);
    x += data->X;
    y += data->Y;
    z += data->Z;
    systickDelay(delay);
  }

  data->X_bias = -x / cnt;
  data->Y_bias = -y / cnt;
  data->Z_bias = -z / cnt;
}
