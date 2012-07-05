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
	        float _corr_vector[3];
	
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
// WARNING: these should actually be matrices

void correction_matrix_pi_controller(const float dt, float corr_vector) {

        int weight_correction;
        int wI_correction; // what to do with this?
        int kP; // proportional gain constant
        int kI; // integral gain constant

        weight_correction = (kP * corr_vector) + (wI_correction + kI * dt * corr_vector);
	// what is weight_correction, exactly? I think this includes the error.
	// we should have weight_correction.x, weight_correction.y, weight_correction.z
}

// pull the gyro data so _state_gyro_last.X etc are available
// correct for drift adjustment by subtract the error from the gyro data and 
// feed that into rotation_matrix_velocity_update as before

        if((ticks - _state_gyro_last_update) >= STATE_GYRO_UPDATE_INTERVAL) {

		if(itg3200GetData(&_state_gyro_last) == i2c_ok) {
	                uint32_t tick_diff = systickGetTicks() - _last_gyro_update_ticks;
	
        	        // Set dt in seconds
                	float dt;
	                if(_last_gyro_update_ticks == 0)
	       	                 dt = 0;
	     	        else
	       	                 dt = tick_diff / 1000.0;

			// element by element, subtract error from gyro vector
			_state_gyro_last.X =- weight_correction.X;
			_state_gyro_last.Y =- weight_correction.Y;
			_state_gyro_last.Z =- weight_correction.Z;

	                // Update rotation matrix
	                // transforms from body frame readings from gyro to world frame readings

	rotation_matrix_velocity_update(rotation_b_to_i, _state_gyro_last.X, _state_gyro_last.Y, _state_gyro_last.Z, dt);

void rotation_matrix_velocity_update(float r[][3], float roll, float pitch, float yaw, float dt) {
        float r_dth_dt[3][3];
        yaw = yaw*dt;
        pitch = pitch*dt;
        roll = roll*dt;

        // Row 1
        r_dth_dt[0][0] = 1;
        r_dth_dt[0][1] = -yaw;
        r_dth_dt[0][2] = pitch;
        // Row 2
        r_dth_dt[1][0] = yaw;
        r_dth_dt[1][1] = 1;
        r_dth_dt[1][2] = -roll;
        // Row 3
        r_dth_dt[2][0] = -pitch;
        r_dth_dt[2][1] = roll;
        r_dth_dt[2][2] = 1;

        float tmp_matrix[3][3];

        matrix_3_3_multiply(r, r_dth_dt, tmp_matrix);
        matrix_3_3_copy(tmp_matrix, r);

        rotation_matrix_normalize(r);
         

       // Update last update ticks
        _last_gyro_update_ticks = systickGetTicks();
        } else
                command_send(COMMAND_ERROR, (unsigned char*)"Invalid gyro read.", 18);

        inertial_needs_update = 1;

         _state_gyro_last_update = ticks;
        }
        ticks = systickGetTicks();
        if(_state_send_interval && (ticks - _state_send_last) >= _state_send_interval) {
                state_send();
                _state_send_last = ticks;
        }
}



