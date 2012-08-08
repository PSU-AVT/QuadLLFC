#include "maxbotixs.h"

void init_maxbotix()
{
        //Enable TMR_TMR32B0
        SCB_SYSAHBCLKCTRL |= 0x200;
        TMR_TMR32B0TCR |= 0x1;
        //Set the prescaler for 24Mhz
        TMR_TMR32B0PR |= 0x2;
        //
        Htick_count = 0;
        Ltick_count = 0;
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

float measure_maxbotix_in(void)
{
        if (Ltick_count <= Htick_count) 
        {
                //return (((Ltick_count + 0xEA57) - Htick_count)/timerSpeed)/uSperInch;
                return -99.0;
        }
        else
        {
                return ((Ltick_count - Htick_count)/timerSpeed)/uSperInch;
        }
}

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
                  //tick_count = systickGetTicks();
                  Htick_count = TMR_TMR32B0TC;
          }
          else
          {
                  //tick_count = systickGetTicks() - tick_count;
                  Ltick_count = TMR_TMR32B0TC;
          }
  }
  return;
}
