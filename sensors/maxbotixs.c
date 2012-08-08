#include "maxbotixs.h"

void init_maxbotix()
{
        SCB_SYSAHBCLKCTRL |= 0x600;
        TMR_TMR32B0TCR |= 0x1;
        TMR_TMR32B0PR |= 0x2;
        data_good = 0;
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
        //return ((Ltick_count - Htick_count)/timerSpeed)/uSperInch;
}

float measure_maxbotix_cm(void)
{
        return (measure_maxbotix_in())*2.54; //magic number
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
                  data_good = 0;
          }
          else
          {
                  //tick_count = systickGetTicks() - tick_count;
                  data_good = 1;
                  Ltick_count = TMR_TMR32B0TC;
          }
  }
  return;
}
