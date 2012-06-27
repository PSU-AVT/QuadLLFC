#include "../core/gpio.h"
#include "../core/systick.h"

volatile uint32_t Htick_count;
volatile uint32_t Ltick_count;
volatile uint32_t data_good;

#define uSperInch 147
#define timerSpeed 36.0 //In MHz


void init_sensor();

float measure(void);
