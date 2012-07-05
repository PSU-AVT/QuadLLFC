void state_update(void) {

        uint32_t ticks = systickGetTicks();

// pull accel data
// make _state_accel_last.X, etc. available for use and update ticks

	if((ticks - _state_accel_last_update) >= STATE_ACCEL_UPDATE_INTERVAL) {

		if(adxl345GetData(&_state_accel_last) == i2c_ok) {
			uint32_t accel_tick_diff = systickGetTicks() - _last_accel_update_ticks;

			// Set dt in seconds
			float accel_dt;
			if(_last_accel_update_ticks == 0)
				accel_dt = 0;
			else
				accel_dt = accel_tick_diff / 1000.0;
		}

// pull mag data
// doesn't exist yet

	if((ticks - _state_mag_last_update) >= STATE_MAG_UPDATE_INTERVAL) {

		if(hmc5883lGetData(&_state_mag_last) == i2c_ok) {
			uint32_t mag_tick_diff = systickGetTicks() - _last_mag_update_ticks;

			// Set dt in seconds
			float mag_dt;
			if(_last_mag_update_ticks == 0)
				mag_dt = 0;
			else
				mag_dt = mag_tick_diff / 1000.0;
		}

// using existing r_matrix, accel data, magdata -- find total_correction

	void find_total_correction_vector()
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
                	temp = weight_rollpitch * RP_corrplane[i];
	                _corr_vector[i] = temp + weight_yaw * Y_corrplane[i];
        	}
	}

// using total_correction and available globals, find the error
// PI controller creates value to correct for the determined error 
// in the rotation correction matrix

void correction_matrix_pi_controller(float dt) {

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

// pull the gyro data so _state_gyro_last.X etc are available
// correct for drift adjustment by subtract the error from the gyro data and 
// feed that into rotation_matrix_velocity_update as before

        if((ticks - _state_gyro_last_update) >= STATE_GYRO_UPDATE_INTERVAL) {

        void state_update_from_gyro(void) { 
        // i2c_ok is in the itg3200.h, enum 
        // we did this for human-readbility 

		if(itg3200GetData(&_state_gyro_last) == i2c_ok) {
	                uint32_t tick_diff = systickGetTicks() - _last_gyro_update_ticks;
	
        	        // Set dt in seconds
                	float gyro_dt;
	                if(_last_gyro_update_ticks == 0)
	       	                 gyro_dt = 0;
	     	        else
	       	                 gyro_dt = tick_diff / 1000.0;

			// element by element, subtract error from gyro vector
			_state_gyro_last.X -= _gyro_error[1];
			_state_gyro_last.Y -= _gyro_error[2];
			_state_gyro_last.Z -= _gyro_error[3];

	                // Update rotation matrix
	                // transforms from body frame readings from gyro to world frame readings

	rotation_matrix_velocity_update(rotation_b_to_i, _state_gyro_last.X, _state_gyro_last.Y, _state_gyro_last.Z, gyro_dt);

       // Update last update ticks
        _last_gyro_update_ticks = systickGetTicks();
        } else
                command_send(COMMAND_ERROR, (unsigned char*)"Invalid gyro read.", 18);

	 inertial_needs_update = 1;

}








         _state_gyro_last_update = ticks;
        }
        ticks = systickGetTicks();
        if(_state_send_interval && (ticks - _state_send_last) >= _state_send_interval) {
                state_send();
                _state_send_last = ticks;
        }
}



