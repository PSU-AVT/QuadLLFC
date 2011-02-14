/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Gregory Haynes
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

/**
 * This ADC module multiplexes across selected pins using the ADC interrupt
 */

#include "../config.h"

enum ADC_PIN_T
{
	ADC_PIN0 = 1,
	ADC_PIN1 = 2,
	ADC_PIN2 = 4,
	ADC_PIN3 = 8,
	ADC_PIN4 = 16,
	ADC_PIN5 = 32,
	ADC_PIN6 = 64,
	ADC_PIN7 = 128
};

#define ADC_PIN_CNT 8
#define ADC_MAX_PINVAL (1 << ADC_PIN_CNT)
#define ADC_RESULT_INVALID 2048

#define adcIsResultValid(VAL) (VAL & ADC_RESULT_INVALID)

uint16_t adcGetVal(uint16_t pin);
uint8_t adcPinToNdx(uint16_t pin);
uint16_t adcGetNdxVal(uint8_t ndx);
void adcStart(void);
void adcSelectPins(int pin);
void adcInit(int pins);
