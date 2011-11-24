/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef MATRIX_H
#define MATRIX_H

void matrix_3_3_multiply(float a[][3], float b[][3], float result[][3]);
void matrix_3_3_copy(float src[][3], float dest[][3]);

#endif
