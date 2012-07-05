#include "filter.h"
#include "sensors/itg3200.h"
#include "sensors/adxl345.h"
#include "sensors/hmc5883l.h"
#include "core/systick.h"
#include "commands.h"
#include "logging.h"

// Sensor fusion using an accelerometer and magenetometer to correct the gyro drift.

void filter_get_accel_data();

void filter_get_mag_data();

void filter_find_total_correction_vector();

void filter_correction_matrix_pi_controller();

void filter_get_and_correct_gyro_data();

/*********************************************************************/
/* everything below this line is still a mix of __new.c and filter.c */

// using the accelerometer and magnetometer data,
// calculate the total required correction vector

void find_total_correction_vector() 
{
	float corr_vector[3];

	const int weight_rollpitch = 0;
	float rollpitch_corrplane[3];
	rollpitch_corrplane = {AccelData.x,AccelData.y,AccelData.z};

	int weight_yaw;
	float yaw_corrplane[3];
	Y_corrplane = {MagData.x,MagData.y,MagData.z};

	int temp = 0;
	for (i=0; i<3; i++)
	{
                temp = weight_rollpitch * rollpitch_corrplane[i];
		corr_vector[i] = temp + weight_yaw * yaw_corrplane[i];
	}

}

// PI controller creates value to correct for the determined error 
// in the rotation correction matrix

void correction_matrix_pi_controller(const float dt, float corr_vector) {

	int w_correction;
	int wI_correction; // what to do with this?
	int kP;
		int kI;

	w_correction = (kP * corr_vector) + (wI_correction + kI * dt * corr_vector);
}

// drift_adjustment
// pass w into rotation_matrix_velocity_update()

void drift_adjustment() {
	
// error_correction = gyroscope - adjustment

}

