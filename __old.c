void state_update(void) {
        uint32_t ticks = systickGetTicks();

        if((ticks - _state_gyro_last_update) >= STATE_GYRO_UPDATE_INTERVAL) {

		if(itg3200GetData(&_state_gyro_last) == i2c_ok) {
	                uint32_t tick_diff = systickGetTicks() - _last_gyro_update_ticks;
	
        	        // Set dt in seconds
                	float dt;
	                if(_last_gyro_update_ticks == 0)
	       	                 dt = 0;
	     	        else
	       	                 dt = tick_diff / 1000.0;

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



