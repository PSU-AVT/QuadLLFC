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


//This reads the distance value returned by the Maxbotixs sonar sensor. It uses
//the Pulse Width output from the sensor.
#include "../core/gpio.h"

#define uSperInch 147 //micro seconds per inch - given by the data sheet
#define timerSpeed 24.0 //In MHz

//This init funtion needs to be called before any of the measure functions to
//get valid data back. You should let the sensor run for 2 cycles (1/10th of 
//a second) before trusting the measured data.
//Initializes the sensor on port 0.6
//Todo this it initializes TMR_TMR32B0 at 24MHz
//it does not initialize any of the timer output pins or match/pwm features
void init_maxbotix(void);

//Returns the distance in inches
//if we call this function in the middle of an update it will return the last
//valid distance reading
float measure_maxbotix_in(void);

//Returns the distance in CM
//if we call this function in the middle of an update it will return the last
//valid distance reading
float measure_maxbotix_cm(void);
