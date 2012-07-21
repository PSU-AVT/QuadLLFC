#include "../core/gpio.h"
#include "../core/systick.h"

volatile uint32_t Htick_count;
volatile uint32_t Ltick_count;
volatile uint32_t data_good;

#define uSperInch 147
#define timerSpeed 24.0 //In MHz
#define zzzi uSperInch * timerSpeed

//Initializes the sensor on port 0.6
void init_sensor();

//Returns the distance in inches
float measure(void);
