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
