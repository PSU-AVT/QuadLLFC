/*
 * i2cHelper.h
 *
 *  Created on: Oct 13, 2011
 *      Author: Eric Dinger
 */

#ifndef I2CHELPER_H_
#define I2CHELPER_H_
#include "i2c.h"

typedef enum {
	i2c_ok,
	i2c_init,
	i2c_busy,
	i2c_error_last
} i2c_error;

#endif
