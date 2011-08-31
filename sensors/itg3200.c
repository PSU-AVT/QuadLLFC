/**************************************************************************/
/*!
    @file     itg3200.c
    @author   M. Ryan (ryanmechatronics.com)
    @date     8 Aug 2010
    @version  0.10

    @section DESCRIPTION

    Driver for Invensense 3 axis ITG3200 gyro

    @section Example

    @code

    @endcode

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

#include "itg3200.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

uint32_t i;
uint8_t gyro_init;

static itg3200Error_e itg3200Write8 (uint8_t reg, uint32_t value)
{
  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = itg3200_ADDRESS;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  I2CMasterBuffer[2] = (value & 0xFF);            // Value to write
  i2cEngine();
  return itg3200_ERROR_OK;
}


static itg3200Error_e itg3200Read8(uint8_t reg, int *value)
{
  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 0;
  I2CReadLength = 1;
  I2CMasterBuffer[0] = itg3200_ADDRESS;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = itg3200_ADDRESS | itg3200_READBIT;
  if(!i2cEngine())
	  return itg3200_ERROR_LAST;

  // Shift values to create properly formed integer
  *value = I2CSlaveBuffer[0];

  return itg3200_ERROR_OK;
}




/**************************************************************************/
/*!
    @brief  Initialises the I2C block
*/
/**************************************************************************/
itg3200Error_e itg3200Init(void)
{
  itg3200Error_e response_error= itg3200_ERROR_OK;
  // Initialise I2C
  if (i2cInit(I2CMODE_MASTER) == FALSE)
  {
    return itg3200_ERROR_I2CINIT;    /* Fatal error */
  }

  response_error = itg3200Write8 (itg3200_REGISTER_CONFIG_DLPF, 0x19);
  if (response_error != itg3200_ERROR_OK)
	  return response_error;

  response_error = itg3200Write8 (itg3200_REGISTER_CONFIG_PMU, 0x01);

  if (response_error != itg3200_ERROR_OK)
	  return response_error;

  response_error = itg3200Write8 (itg3200_REGISTER_CONFIG_SMPLDIV, 0x07);

  if (response_error == itg3200_ERROR_OK)
  {
	  gyro_init = 1;
  } else {
	  gyro_init = 0;
  }
  return response_error;
}

/**************************************************************************/
/*!
    @brief  Reads the gyro data set (full)

    @note   This method will read the entire data set from the gyro and return
                        it in the gyro structure.
*/
/**************************************************************************/
itg3200Error_e itg3200GetData (GyroData *data)
{
  uint8_t i, tmpdat[8];
  int dat, tmp;
  itg3200Error_e error = itg3200_ERROR_OK;
  uint8_t err_exp = 0;


    if (!gyro_init)
        itg3200Init();

        //error = itg3200Read8 (itg3200_REGISTER_WHOAMI, &dat);
        //printf("Who err, val: %d, %d.\n\r", (int) error, (int)dat);

        for (i = 0; i< 8;i++)
        {
                error = itg3200Read8 (itg3200_REGISTER_TEMPH + i, &dat); //fill tmparray with data
                if (!error) tmpdat[i] = dat;
                else err_exp = 1;
        }
                data->raw_temp = (short) ((int) tmpdat[0] << 8) | ((int) tmpdat[1]);
                data->raw_X = (short) ((int) tmpdat[2] << 8) | ((int) tmpdat[3]);
                data->raw_Y = (short) ((int) tmpdat[4] << 8) | ((int) tmpdat[5]);
                data->raw_Z = (short) ((int) tmpdat[6] << 8) | ((int) tmpdat[7]);
        //Note:  Temperature scaling:
        //Offset = -35 degrees, 13200 counts. 280 counts/degrees C.
        data->temp = 35.0 + ((data->raw_temp + 13200)/280.0);
        //Gyro scaling:  14.375 LSB/degrees/sec
        //SO output should be counts / 14.375;
        /*
        data->X = data->raw_X / 14.375;
        data->Y = data->raw_Y / 14.375;
        data->Z = data->raw_Z / 14.375;
        lets use radians, not degrees
        and multiply, not divide
        */
        data->X = data->raw_X / LSB_CORRECTION;
        data->Y = data->raw_Y / LSB_CORRECTION;
        data->Z = data->raw_Z / LSB_CORRECTION;

        data->X += data->x_bias;
        data->Y += data->y_bias;
        data->Z += data->z_bias;

        if(err_exp)
        	error = itg3200_ERROR_LAST;
        return error;
}

/**************************************************************************/
/*!
    @brief  Reads the gyro who am I register

*/
/**************************************************************************/
unsigned char itg3200_Get_WhoAmI (void)
{
  int dat;
  itg3200Error_e error = itg3200_ERROR_OK;

    if (!gyro_init)
        itg3200Init();

        error = itg3200Read8 (itg3200_REGISTER_WHOAMI, &dat);
        if (dat == 0x69)
        {
          return(1);
        }
          return(0);
}
