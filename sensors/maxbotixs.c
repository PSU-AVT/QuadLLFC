#include "maxbotixs.h"

void init_sensor()
{
        data_good = 0;
        tick_count = 0;
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

void measure(float * dist)
{
        while(!data_good);
        *dist = (tick_count/timerSpeed)/uSperInch;
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
                  tick_count = TMR_TMR16B0TC;
                  data_good = 0;
          }
          else
          {
                  //tick_count = systickGetTicks() - tick_count;
                  data_good = 1;
                  tick_count = TMR_TMR16B0TC - tick_count;
          }
  }
  return;
}
