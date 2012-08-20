/*
 * Copyright (c) 2012 Jenner Hanni <jeh.wicker@gmail.com>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef FILTER_H
#define FILTER_H

void filter_get_accel_data();
void filter_get_mag_data();
void filter_find_total_correction_vector();
void filter_get_gyro_correction_data(float gyro_data);

#endif 
