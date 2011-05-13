#ifndef CONFIG_H
#define CONFIG_H

#include "lpc134x.h"

#define CFG_PI                      (3.141)

// This doesn't actually effect clock speed
#define CFG_CPU_CCLK                (72000000)

#define CFG_PWM_DEFAULT_PULSEWIDTH  (CFG_CPU_CCLK / 1000)
#define CFG_PWM_DEFAULT_DUTYCYCLE   (50)

#define CFG_UART_BAUDRATE           (9600)
#define CFG_UART_BUFSIZE            (512)

#define CFG_CTL_K_PREDICT           (10)
#define CFG_CTL_K_UPDATE            (2)

#define CFG_GYRO_UPDATE_MSECS      (4) // msecs between sensor updates
#define CFG_RESPONSE_UPDATE_MSECS  (50) // msecs between responding to state
#define CFG_STATE_OUTPUT_MSECS     (200)

#define CFG_ACCELERO_X_ADC_PIN     ADC_PIN3
#define CFG_ACCELERO_Y_ADC_PIN     ADC_PIN5
#define CFG_ACCELERO_Z_ADC_PIN     ADC_PIN7

#define CFG_ROLL_BASE_VAL          (401.03)
#define CFG_PITCH_BASE_VAL         (401.03)
#define CFG_YAW_BASE_VAL           (401.03)

#define CFG_ACCELERO_X_BASE_VAL    (505.5)
#define CFG_ACCELERO_Y_BASE_VAL    (510.6)
#define CFG_ACCELERO_Z_BASE_VAL    (401)

#define CFG_ACCELERO_X_RANGE       (104.2)

#define CFG_ROLL_UPDATE_MSECS      (2)
#define CFG_PITCH_UPDATE_MSECS     (2)
#define CFG_YAW_UPDATE_MSECS       (2)

#define CFG_ROLL_GYRO_FILTER_MSECS (10)
#define CFG_PITCH_GYRO_FILTER_MSECS (5)
#define CFG_YAW_GYRO_FILTER_MSECS  (5)
#define CFG_GYRO_FILTER_ALPHA      (0.1f)

#define CFG_PITCH_BIAS_CORRECT_MSECS (1000)
#define CFG_ROLL_BIAS_CORRECT_MSECS  (1000)

#define CFG_ACCELERO_FILTER_MSECS  (10)
#define CFG_ACCELERO_FILTER_ALPHA  (0.1f)

#define CFG_GYRO_ZERO_WIDTH        (1)

#define CFG_ESC_CNT 4
#define CFG_MOTOR_CNT CFG_ESC_CNT
#define CFG_MOTOR_DEFAULT_MAX_THRUST 20000
#define CFG_MOTOR_DEFAULT_MIN_THRUST 36000
#define CFG_MOTOR_DEFAULT_DUTY_CYCLE 0

#define MOTOR_LF 1
#define MOTOR_LR 0
#define MOTOR_RF 2
#define MOTOR_RR 3

#endif
