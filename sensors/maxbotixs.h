#include "../core/gpio.h"
#include "../core/systick.h"

volatile uint32_t Htick_count;
volatile uint32_t Ltick_count;

#define uSperInch 147 //micro seconds per inch - given by the data sheet
#define timerSpeed 24.0 //In MHz

//Initializes the sensor on port 0.6
//Todo this it initializes TMR_TMR32B0 at 24MHz
//it does not initialize any of the timer output pins or match/pwm features
void init_maxbotix(void);

//Returns the distance in inches
float measure_maxbotix_in(void);

//Returns the distance in CM
float measure_maxbotix_cm(void);
