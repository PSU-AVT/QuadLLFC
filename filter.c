/* Sensor fusion using an accelerometer and magenetometer to correct the gyro drift.
 *
 * This is a working file.
 */

// done by others

calibrate gyroscope - itg3200Calibrate
calibrate accelerometer - adx345Calibrate

// the next pieces are not done yet

read gyroscope
read accelerometer

// calibrate magnetometer
//	calculate heading

// drift_detection
//	find roll-pitch error
//	find heading error

int RP_error;

// using the accelerometer and magnetometer data,
// calculate the total required correction vector

void find_total_correction_vector() {
	float corr_vector[3];

	int W_RP;
	float RP_corr_plane[3];
	RP_corr_plane = {AccelData.x,AccelData.y,AccelData.z};

	int W_Y;
	float Y_corr_plane[3];
	Y_corr_plane = {MagData.x,MagData.y,MagData.z};

	corr_vector = (W_RP * RP_corr_plane) + (W_Y * Y_corr_plane);
}

// PI controller creates value to correct for the determined error 
// in the rotation correction matrix

void correction_matrix_pi_controller(float dt, float corr_vector) {

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

