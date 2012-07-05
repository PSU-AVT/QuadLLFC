#ifndef FILTER_H
#define FILTER_H

void filter_get_accel_data(float accel_dt);
void filter_get_mag_data(float mag_dt);
void filter_find_total_correction_vector();
void filter_correction_matrix_pi_controller();
void filter_get_gyro_correction_data(float dt);

#endif 
