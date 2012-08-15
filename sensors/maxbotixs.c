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
#include "maxbotixs.h"

volatile uint32_t Htick_count;
volatile uint32_t Ltick_count;
volatile uint8_t  dataGood;
static float distance;

void init_maxbotix()
{
        //The following configures the timer
        //Enable TMR_TMR32B0
        SCB_SYSAHBCLKCTRL |= 0x200;
        TMR_TMR32B0TCR |= 0x1;
        //Set the prescaler for 24Mhz
        TMR_TMR32B0PR |= 0x2;
        
        //clear the variables
        Htick_count = 0;
        Ltick_count = 0;
        dataGood = 0;
        distance = 0;
        
        //Configure the pins and the interupt
        //Set port.pin to an input 
        gpioSetDir(0, 6, gpioDirection_Input);
        //Disable the internal pullup/down resistor
        gpioSetPullup(&IOCON_PIO0_6, gpioPullupMode_Inactive);
        //set up an interupt
        gpioSetInterrupt(0,6,
                        gpioInterruptSense_Edge,
                        gpioInterruptEdge_Double,
                        gpioInterruptEvent_ActiveHigh);
        gpioIntEnable(0,6);
}

//Returns the distance in inches
float measure_maxbotix_in(void)
{
        if (dataGood)
        {
                if (Ltick_count <= Htick_count) 
                {
                        distance = ((((uint64_t)Ltick_count + 0xFFFFFFFF) - Htick_count)/timerSpeed)/uSperInch;
                }
                else
                {
                        distance = ((Ltick_count - Htick_count)/timerSpeed)/uSperInch;
                }
        }
        return distance;
}

//Returns the distance in cm
float measure_maxbotix_cm(void)
{
        return (measure_maxbotix_in())*2.54; //2.54 * inch = cm
}
        
void PIOINT0_IRQHandler(void)
{
  uint32_t regVal;

  regVal = gpioIntStatus(0, 6);
  if (regVal)
  {
          uint32_t status = gpioGetValue(0,6);
          gpioIntClear(0, 6);
          if (status)
          {
                  Htick_count = TMR_TMR32B0TC;
                  dataGood = 0;
          }
          else
          {
                  Ltick_count = TMR_TMR32B0TC;
                  dataGood = 1;
          }
  }
  return;
}
