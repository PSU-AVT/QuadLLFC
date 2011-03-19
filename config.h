#ifndef CONFIG_H
#define CONFIG_H

#include "lpc134x.h"

// This doesn't actually effect clock speed
#define CFG_CPU_CCLK                (72000000)

#define CFG_PWM_DEFAULT_PULSEWIDTH  (CFG_CPU_CCLK / 1000)
#define CFG_PWM_DEFAULT_DUTYCYCLE   (50)

#define CFG_UART_BAUDRATE           (9600)
#define CFG_UART_BUFSIZE            (512)

#define CFG_CTL_K_PREDICT           (10)
#define CFG_CTL_K_UPDATE            (2)

#define CFG_STATE_UPDATE_MSECS     (80) // msecs between sensor updates
#define CFG_RESPONSE_UPDATE_MSECS  (10) // msecs between responding to state
#define CFG_STATE_OUTPUT_MSECS     (200)

#define CFG_ROLL_ADC_PIN ADC_PIN0
#define CFG_PITCH_ADC_PIN ADC_PIN1
#define CFG_YAW_ADC_PIN ADC_PIN2
#define CFG_GYRO_ZERO_WIDTH        (1)

#define CFG_ESC_CNT 4
#define CFG_MOTOR_CNT CFG_ESC_CNT
#define CFG_MOTOR_DEFAULT_MAX_THRUST 30500
#define CFG_MOTOR_DEFAULT_MIN_THRUST 36000
#define CFG_MOTOR_DEFAULT_DUTY_CYCLE 0

#endif
