#include "pwm.h"

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#define AHBCLKCTRL_CT16B0 0x80;
#define AHBCLKCTRL_CT16B1 0x100;

#define TIMER_16_0_MR3_RESET_ENABLED 0x400;
#define TIMER_16_0_EMC0_TOGGLE 0x30
#define TIMER_16_0_EMR_EM0 0x1
#define TIMER_16_0_COUNTERENABLE 0x1

#define TIMER_16_1_MR3_RESET_ENABLED 0x400;
#define TIMER_16_1_EMC0_TOGGLE 0x30
#define TIMER_16_1_EMR_EM0 0x1
#define TIMER_16_1_COUNTERENABLE 0x1

void pwm_16_init_timers(PWM_TIMER timers) {
    if(timers & PWM_TIMER_16_0) {
        /* Enable Timer */
    	LPC_SYSCON->SYSAHBCLKCTRL |= AHBCLKCTRL_CT16B0;

    	/* Match compare register */
    	LPC_TMR16B0->MCR = TIMER_16_0_MR3_RESET_ENABLED;

        /* External Match Register Settings for PWM */
    	LPC_TMR16B0->EMR = TIMER_16_0_EMC0_TOGGLE | TIMER_16_0_EMR_EM0;

        /* Disable counter */
    	LPC_TMR16B0->TCR &= TIMER_16_0_COUNTERENABLE;
    }
    if(timers & PWM_TIMER_16_1) {
        /* Enable Timer */
    	LPC_SYSCON->SYSAHBCLKCTRL |= AHBCLKCTRL_CT16B1;

    	/* Match compare register */
    	LPC_TMR16B1->MCR = TIMER_16_1_MR3_RESET_ENABLED;

        /* External Match Register Settings for PWM */
    	LPC_TMR16B1->EMR = TIMER_16_1_EMC0_TOGGLE | TIMER_16_1_EMR_EM0;

        /* Disable counter */
    	LPC_TMR16B1->TCR &= TIMER_16_1_COUNTERENABLE;
    }
}
