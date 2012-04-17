#include "../core/gpio.h"
#include "../core/systick.h"

volatile uint32_t tick_count;


void init_sensor();

void measure(float * dist);
