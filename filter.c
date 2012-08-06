#include "math.h"
#include "state.h"
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

extern float rotation_b_to_i[3][3]; 

static float _corr_vector[3];
float _gyro_error[] = {0,0,0};

// pull accelerometer data
// make _state_accel_last.X, etc. available for use and update ticks

void filter_get_accel_data() 
{
	uint32_t ticks = systickGetTicks();
        if((ticks - _state_accel_last_update) >= FILTER_ACCEL_UPDATE_INTERVAL) 
	{

                if(adxl345GetData(&_state_accel_last) == i2c_ok) {
                	float accel_dt;
                	uint32_t accel_tick_diff = systickGetTicks() - _last_accel_update_ticks;

                        // Set dt in seconds
                        if(_last_accel_update_ticks == 0)
                                accel_dt = 0;
                        else
                                accel_dt = accel_tick_diff / 1000.0;
		}
         }

}

// pull mag data, which doesn't exist yet

/*void filter_get_mag_data()
{
	uint32_t ticks = systickGetTicks();
        if((ticks - _state_mag_last_update) >= FILTER_MAG_UPDATE_INTERVAL) 
	{
                if(hmc5883lGetData(&_state_mag_last) == i2c_ok) 
		{
                        uint32_t mag_tick_diff = systickGetTicks() - _last_mag_update_ticks;
                        float mag_dt;
                        // Set dt in seconds
                        if(_last_mag_update_ticks == 0)
                                mag_dt = 0;
                        else
                                mag_dt = mag_tick_diff / 1000.0;
		}
        }
}*/

// using existing r_matrix, accel data, magdata -- find total_correction

void filter_find_total_correction_vector() 
{
	filter_get_accel_data();
//	filter_get_mag_data();

        float g_ref[3];
        g_ref[0] = _state_accel_last.X;
        g_ref[1] = _state_accel_last.Y;
        g_ref[2] = _state_accel_last.Z;

        const float weight_rollpitch = 1;
        float rollpitch_corrplane[3];
        rollpitch_corrplane[0] = rotation_b_to_i[2][1];
        rollpitch_corrplane[1] = rotation_b_to_i[0][2];
//      rollpitch_corrplane[0] = _state_accel_last.roll;
// 	rollpitch_corrplane[1] = _state_accel_last.pitch;
	rollpitch_corrplane[2] = 1;

//      const float weight_yaw = 1;
//      float yaw_corr_heading;
//      yaw_corr_heading = atan2(_state_mag_last.Y,_state_mag_last.X);

	int i;
        for (i=0; i<3; i++)
        {
            _corr_vector[i] = weight_rollpitch * rollpitch_corrplane[i];
//            _corr_vector[i] = temp + (weight_yaw * yaw_corr_heading);
        }

        state_t test;

        test.roll = _corr_vector[0];
        test.pitch = _corr_vector[1];
        test.yaw = _corr_vector[2];

        command_send(COMMAND_INERTIAL_STATE, (unsigned char*)&test, sizeof(state_t));

}

// using total_correction and available globals, find the error
// PI controller creates value to correct for the determined error 
// in the rotation correction matrix

void filter_get_gyro_correction_data(float gyro_dt)
{
        filter_find_total_correction_vector();

        static float wI_correction = 0; // takes corr_vector and gives gyro_error 
        static float wP_correction = 0; // takes corr_vector and gives gyro_error 

        float kP = 1; // proportional gain constant
        float kI = 1; // integral gain constant

//        float fix_yaw = 0;
	int i;
        float dt = gyro_dt;

        for (i=0; i<3; i++)
        {
//                fix_yaw = kP * _corr_vector[i];
                wP_correction = kP * _corr_vector[i];
                wI_correction = kI * (dt * _corr_vector[i]);
                _gyro_error[i] = wP_correction + wI_correction;
        }
}


