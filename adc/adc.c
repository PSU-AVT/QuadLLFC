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

static volatile int _adc_selected_pins;
static volatile int _adc_selected_pin;
static volatile uint16_t _adc_vals[ADC_PIN_CNT];

static int adc_clkdiv;

#define ADC_CTL_RESET /* Puts ADC into state where only pin needs to be set */\
	ADC_AD0CR = ((adc_clkdiv) |   /* CLKDIV = Fpclk / 1000000 - 1 */\
	             ADC_AD0CR_BURST_SWMODE |                 /* BURST = 0, no BURST, software controlled */\
	             ADC_AD0CR_CLKS_10BITS |                  /* CLKS = 0, 11 clocks/10 bits */\
	             ADC_AD0CR_START_NOSTART |                /* START = 0 A/D conversion stops */\
	             ADC_AD0CR_EDGE_RISING);                  /* EDGE = 0 (CAP/MAT signal falling, trigger A/D conversion) */

#define ADC_GET_VAL(X) ((X >> 6) & 0x3FF)

void adcSelectNextPin(void);
void adcCtlSetSelectedPin(void);

uint8_t adcPinToNdx(uint16_t pin)
{
	int i = -1;
	if(!pin)
		return 0;
	while(pin)
	{
		i++;
		pin = pin >> 1;
	}
	return i;
}

uint16_t adcGetVal(uint16_t pin)
{
	uint8_t i = adcPinToNdx(pin);
	return _adc_vals[i];
}

uint16_t adcGetNdxVal(uint8_t ndx)
{
	return _adc_vals[ndx];
}

void ADC_IRQHandler(void)
{
	if(ADC_AD0STAT & ADC_PIN0) // This works...trust me
		_adc_vals[0] = ADC_GET_VAL(ADC_AD0DR0);
	if(ADC_AD0STAT & ADC_PIN1)
		_adc_vals[1] = ADC_GET_VAL(ADC_AD0DR1);
	if(ADC_AD0STAT & ADC_PIN2)
		_adc_vals[2] = ADC_GET_VAL(ADC_AD0DR2);
	if(ADC_AD0STAT & ADC_PIN3)
		_adc_vals[3] = ADC_GET_VAL(ADC_AD0DR3);
	if(ADC_AD0STAT & ADC_PIN4)
		_adc_vals[4] = ADC_GET_VAL(ADC_AD0DR4);
	if(ADC_AD0STAT & ADC_PIN5)
		_adc_vals[5] = ADC_GET_VAL(ADC_AD0DR5);
	if(ADC_AD0STAT & ADC_PIN6)
		_adc_vals[6] = ADC_GET_VAL(ADC_AD0DR6);
	if(ADC_AD0STAT & ADC_PIN7)
		_adc_vals[7] = ADC_GET_VAL(ADC_AD0DR7);

	adcSelectNextPin();
	ADC_AD0CR |= ADC_AD0CR_START_STARTNOW;
}

/* Performs the multiplexing */
void adcSelectNextPin(void)
{
	do
	{
		_adc_selected_pin = _adc_selected_pin << 1;
		if(_adc_selected_pin > ADC_MAX_PINVAL)
			_adc_selected_pin = 1;
	} while(!(_adc_selected_pin & _adc_selected_pins));

	adcCtlSetSelectedPin();
}

/* Set ADC registers to read from selected pin */
void adcCtlSetSelectedPin(void)
{
	ADC_CTL_RESET
	ADC_AD0CR |= _adc_selected_pin; /* This works...trust me */
}

void adcStart()
{
	int i;

	if(!(_adc_selected_pins & 1))
		adcSelectNextPin();
	else
		_adc_selected_pin = 1;

	/* Set all results to invalid */
	for(i = 0;i < ADC_PIN_CNT;i++)
		_adc_vals[i] = ADC_RESULT_INVALID;

	adcCtlSetSelectedPin();

	/* Start the ADC */
	ADC_AD0CR |= ADC_AD0CR_START_STARTNOW;
}

void adcSelectPins(int pins)
{
	_adc_selected_pins |= pins;
}

void adcInit(int pins)
{
	adc_clkdiv = ((CFG_CPU_CCLK / SCB_SYSAHBCLKDIV) / 1000000 - 1 ) << 8;

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
	if(pins & ADC_PIN2)
	{
		IOCON_JTAG_TDO_PIO1_1 &=  ~(IOCON_JTAG_TDO_PIO1_1_ADMODE_MASK |
		                            IOCON_JTAG_TDO_PIO1_1_FUNC_MASK |
		                            IOCON_JTAG_TDO_PIO1_1_MODE_MASK);
		IOCON_JTAG_TDO_PIO1_1 |=   (IOCON_JTAG_TDO_PIO1_1_FUNC_AD2 &
		                            IOCON_JTAG_TDO_PIO1_1_ADMODE_ANALOG);
	}
	if(pins & ADC_PIN3)
	{
		IOCON_JTAG_nTRST_PIO1_2 &=  ~(IOCON_JTAG_nTRST_PIO1_2_ADMODE_MASK |
		                              IOCON_JTAG_nTRST_PIO1_2_FUNC_MASK |
		                              IOCON_JTAG_nTRST_PIO1_2_MODE_MASK);
		IOCON_JTAG_nTRST_PIO1_2 |=   (IOCON_JTAG_nTRST_PIO1_2_FUNC_AD3 &
		                              IOCON_JTAG_nTRST_PIO1_2_ADMODE_ANALOG);
	}
	if(pins & ADC_PIN4)
	{
		// SWD pin
	}
	if(pins & ADC_PIN5)
	{
		IOCON_PIO1_4 &=  ~(IOCON_PIO1_4_ADMODE_MASK |
		                   IOCON_PIO1_4_FUNC_MASK |
		                   IOCON_PIO1_4_MODE_MASK);
		IOCON_PIO1_4 |=   (IOCON_PIO1_4_FUNC_AD5 &
		                   IOCON_PIO1_4_ADMODE_ANALOG);
	}
	if(pins & ADC_PIN6)
	{
		IOCON_PIO1_10 &=  ~(IOCON_PIO1_10_ADMODE_MASK |
		                    IOCON_PIO1_10_FUNC_MASK |
		                    IOCON_PIO1_10_MODE_MASK);
		IOCON_PIO1_10 |=   (IOCON_PIO1_10_FUNC_AD6 &
		                    IOCON_PIO1_10_ADMODE_ANALOG);
	}
	if(pins & ADC_PIN7)
	{
		IOCON_PIO1_11 &=  ~(IOCON_PIO1_11_ADMODE_MASK |
		                    IOCON_PIO1_11_FUNC_MASK |
		                    IOCON_PIO1_11_MODE_MASK);
		IOCON_PIO1_11 |=   (IOCON_PIO1_11_FUNC_AD7 &
		                    IOCON_PIO1_11_ADMODE_ANALOG);
	}

	// Enable ADC Interrupts
	NVIC_EnableIRQ(ADC_IRQn);
}
