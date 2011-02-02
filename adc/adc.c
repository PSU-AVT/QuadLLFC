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

#include "adc.h"

static int _adc_selected_pins;
static int _adc_selected_pin;
static unsigned int _adc_vals[ADC_PIN_CNT];

#define ADC_CTL_RESET /* Puts ADC into state where only pin needs to be set */\
	ADC_AD0CR = ((((CFG_CPU_CCLK / SCB_SYSAHBCLKDIV) / 1000000 - 1 ) << 8) |   /* CLKDIV = Fpclk / 1000000 - 1 */\
	             ADC_AD0CR_BURST_SWMODE |                 /* BURST = 0, no BURST, software controlled */\
	             ADC_AD0CR_CLKS_10BITS |                  /* CLKS = 0, 11 clocks/10 bits */\
	             ADC_AD0CR_START_NOSTART |                /* START = 0 A/D conversion stops */\
	             ADC_AD0CR_EDGE_RISING);                  /* EDGE = 0 (CAP/MAT signal falling, trigger A/D conversion) */

void ADC_IRQHandler(void)
{
	switch(_adc_selected_pin)
	{
	case ADC_PIN0:
		_adc_vals[0] = (*(pREG32(ADC_AD0DR0)));
		break;
	case ADC_PIN1:
		_adc_vals[1] = (*(pREG32(ADC_AD0DR1)));
		break;
	}
}

/* Performs the multiplexing */
void adcSelectNextPin()
{
	do
	{
		_adc_selected_pin = _adc_selected_pin << 1;
		if(_adc_selected_pin > ADC_MAX_PINVAL)
			_adc_selected_pin = 1;
	} while(!(_adc_selected_pin & _adc_selected_pins));
}

/* Set ADC registers to read from selected pin */
void adcCtlSetSelectedPin()
{
	ADC_CTL_RESET
	ADC_AD0CR |= _adc_selected_pin; /* This works...trust me */
}

void adcStart()
{
	int i;

	/* Select the first pin */
	_adc_selected_pin = 1;
	if(!(_adc_selected_pins & 1))
		adcSelectNextPin();

	/* Set all results to invalid */
	for(i = 0;i < ADC_PIN_CNT;i++)
		_adc_vals[i] = ADC_RESULT_INVALID;

	/* Start the ADC */
	ADC_AD0CR |= ADC_AD0CR_START_STARTNOW;
}

void adcSelectPins(int pins)
{
	ADC_CTL_RESET

	_adc_selected_pins = pins;
}

void adcInit(int pins)
{
	/* Enable power to ADC */
	SCB_PDRUNCFG &= ~(SCB_PDRUNCFG_ADC);

	/* Enable ADC clock */
	SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_ADC);

	if(pins & ADC_PIN0)
	{
		IOCON_JTAG_TDI_PIO0_11 &= ~(IOCON_JTAG_TDI_PIO0_11_ADMODE_MASK |
		                            IOCON_JTAG_TDI_PIO0_11_FUNC_MASK |
		                            IOCON_JTAG_TDI_PIO0_11_MODE_MASK);
		IOCON_JTAG_TDI_PIO0_11 |=  (IOCON_JTAG_TDI_PIO0_11_FUNC_AD0 &
		                            IOCON_JTAG_TDI_PIO0_11_ADMODE_ANALOG);
	}
	if(pins & ADC_PIN1)
	{
		IOCON_JTAG_TMS_PIO1_0 &=  ~(IOCON_JTAG_TMS_PIO1_0_ADMODE_MASK |
		                            IOCON_JTAG_TMS_PIO1_0_FUNC_MASK |
		                            IOCON_JTAG_TMS_PIO1_0_MODE_MASK);
		IOCON_JTAG_TMS_PIO1_0 |=   (IOCON_JTAG_TMS_PIO1_0_FUNC_AD1 &
		                            IOCON_JTAG_TMS_PIO1_0_ADMODE_ANALOG);
	}

	// Enable ADC Interrupts
	NVIC_EnableIRQ(ADC_IRQn);
}
