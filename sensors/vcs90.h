/*
 * Author: Eric Dinger <egdinger@cs.pdx.edu>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Eric Dinger
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
 *This is the header for the autopilot voltage/current sensor
 *It does not have a part number or good documentation, and the website
 *no longer works, the data sheet is on sparkfun though
 *We are using the 90amp version
*/
#include "../core/adc.h"

//Assumption adc vref is 3.3 volts
//this would mean 3300/1024 = 3.22 mv/lsb
#define MV_LSB 3.22

//63.69mV/Volt
#define VOLT_SCALE 63.69

//36.60mV/Amp
#define AMP_SCALE 36.60

//Sets up the adc pins to be used by the measure functions
void vcs90_init(ADC_PIN_T v_pin, ADC_PIN_T i_pin);

//Returns the voltage currently measured by the sensor
float vcs90_measure_voltage(void);

//Returns the current currently measured by the sensor
float vcs90_measure_current(void);
