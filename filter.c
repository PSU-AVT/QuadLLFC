#include "filter.h"
#include "sensors/itg3200.h"
#include "sensors/adxl345.h"
#include "sensors/hmc5883l.h"
#include "core/systick.h"
#include "commands.h"
#include "logging.h"

#define FILTER_ACCEL_UPDATE_INTERVAL 5
#define FILTER_MAG_UPDATE_INTERVAL 5

static uint32_t  _state_accel_last_update;
static uint32_t  _last_accel_update_ticks;
static AccelData _state_accel_last;

static uint32_t _state_mag_last_update;
static uint32_t _last_mag_update_ticks;
static MagData  _state_mag_last;

static float _corr_vector[3];
static float _gyro_error[3];i

// pull accelerometer data
// make _state_accel_last.X, etc. available for use and update ticks

void filter_get_accel_data(float accel_dt) 
{
        if((ticks - _state_accel_last_update) >= STATE_ACCEL_UPDATE_INTERVAL) {

                if(adxl345GetData(&_state_accel_last) == i2c_ok) {
                        uint32_t accel_tick_diff = systickGetTicks() - _last_accel_update_ticks;

                        // Set dt in seconds
                        if(_last_accel_update_ticks == 0)
                                accel_dt = 0;
                        else
                                accel_dt = accel_tick_diff / 1000.0;
         }

}

// pull mag data, which doesn't exist yet

void filter_get_mag_data(float mag_dt)
{
        if((ticks - _state_mag_last_update) >= STATE_MAG_UPDATE_INTERVAL) {

                if(hmc5883lGetData(&_state_mag_last) == i2c_ok) {
                        uint32_t mag_tick_diff = systickGetTicks() - _last_mag_update_ticks;

                        // Set dt in seconds
                        if(_last_mag_update_ticks == 0)
                                mag_dt = 0;
                        else
                                mag_dt = mag_tick_diff / 1000.0;
        }
}

// using existing r_matrix, accel data, magdata -- find total_correction

void filter_find_total_correction_vector() 
{
        const int weight_rollpitch = 0;
        float rollpitch_corrplane[3];
        rollpitch_corrplane = {AccelData.X,AccelData.Y,AccelData.Z};

        int weight_yaw = 0;
        float yaw_corrplane[3];
        yaw_corrplane = {MagData.X,MagData.Y,MagData.Z};

        int temp = 0;
        for (i=0; i<3; i++)
        {
            temp = weight_rollpitch * rollpitch_corrplane[i];
            _corr_vector[i] = temp + (weight_yaw * yaw_corrplane[i]);
        }
}

// using total_correction and available globals, find the error
// PI controller creates value to correct for the determined error 
// in the rotation correction matrix

void filter_get_gyro_correction_data(float dt)
{
        int wI_correction = 1; // what is the value of this? no idea.
        int kP = 1; // proportional gain constant
        int kI = 1; // integral gain constant

        int temp_yaw = 0;
        int temp_rollpitch = 0;

        for (i=0; i<3; i++)
        {
                fix_yaw = kP * corr_vector[i]
                fix_rollpitch = kI * dt * corr_vector[i];
                fix_rollpitch = temp2 + wI_correction;
                _gyro_error[i] = fix_yaw + fix_rollpitch;
        }
}


