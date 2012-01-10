/*
 * adxl345.h
 *
 *  Created on: Oct 4, 2011
 *      Author: Eric Dinger
 */

#ifndef ADXL345_H_
#define ADXL345_H_

//#include "../config.h"
#include "../lpc134x.h"
#include "../core/i2cError.h"
#include "../core/i2c.h"
#include "../commands.h"

#define adxl345_LSB_CORRECTION .0039 //For the 2g range

#define adxl345_ADDRESS (0xA6) //Using the alternate adress as specified in the data sheet was 0x53
#define adxl345_READBIT (0x01)

//Device registers - names correspond to the register names in the datasheets.
#define adxl345_REGISTER_DEVICE_ID (0x00)
#define adxl345_REGISTER_THRESH_TAP (0x1D)
#define adxl345_REGISTER_OFSX (0x1E)
#define adxl345_REGISTER_OFSY (0x1F)
#define adxl345_REGISTER_OFSZ (0x20)
#define adxl345_REGISTER_DUR (0x21)
#define adxl345_REGISTER_LATENT (0x22)
#define adxl345_REGISTER_WINDOW (0x23)
#define adxl345_REGISTER_THRESH_ACT (0x24)
#define adxl345_REGISTER_THRESH_INACT (0x25)
#define adxl345_REGISTER_TIME_INACT (0x26)
#define adxl345_REGISTER_ACT_INACT_CTL (0x27)
#define adxl345_REGISTER_THRESH_FF (0x28)
#define adxl345_REGISTER_TIME_FF (0x29)
#define adxl345_REGISTER_TAP_AXES (0x2A)
#define adxl345_REGISTER_ACT_TAP_STATUS (0x2B)
#define adxl345_REGISTER_BW_RATE (0x2C)
#define adxl345_REGISTER_POWER_CTL (0x2D)
#define adxl345_REGISTER_INT_ENABLE (0x2E)
#define adxl345_REGISTER_INT_MAP (0x2F)
#define adxl345_REGISTER_INT_SOURCE (0x30)
#define adxl345_REGISTER_DATA_FORMAT (0x31)
#define adxl345_REGISTER_DATAX0 (0x32)
#define adxl345_REGISTER_DATAX1 (0x33)
#define adxl345_REGISTER_DATAY0 (0x34)
#define adxl345_REGISTER_DATAY1 (0x35)
#define adxl345_REGISTER_DATAZ0 (0x36)
#define adxl345_REGISTER_DATAZ1 (0x37)
#define adxl345_REGISTER_FIFO_CTL (0x38)
#define adxl345_REGISTER_FIFO_STATUS (0x39)

//The following are the values that we wish to be in the named register after initializing the accelerometer
#define adxl345_REGISTER_CONFIG_POWER_CTL (0x08) //Set the Measure bit to begin measuring.
//The rest of the registers have default values that work - for now.

uint8_t accel_init;

//unsure if we need these or if they are only for memory optimization?
//#pragma pack (push)
//#pragma pack (1)
typedef struct
{
	//Should be x and y? Check with Greg.
	float x;
	float y;
	float z;

	float roll;
	float pitch;
        
        //These shorts HAVE TO COME AFTER THE FLOATS!
        //Otherwise there is a byte alignment error. WTF CODERED?!
        short raw_x;
	short raw_y;
	short raw_z;
}
AccelData;

//Initializes the accel and starts the sending of data
i2c_error adxl345_Init(void);

//Reads the data from the sensor and fills in the AccelData structure passed in.
i2c_error adxl345GetData(AccelData *data);

#endif /* ADXL345_H_ */
