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
