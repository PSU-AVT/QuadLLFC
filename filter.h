#ifndef FILTER_H
#define FILTER_H

void filter_get_accel_data();
void filter_get_mag_data();
void filter_find_total_correction_vector();
void filter_correction_matrix_pi_controller();
void filter_get_and_correct_gyro_data();

#endif 
