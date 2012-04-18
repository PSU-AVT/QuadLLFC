#include "../core/gpio.h"
#include "../core/systick.h"

volatile uint32_t tick_count;
volatile uint32_t data_good;

#define uSperInch 147
#define timerSpeed 36 //In MHz


void init_sensor();

void measure(float * dist);
