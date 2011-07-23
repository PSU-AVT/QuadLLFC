#include "matrix.h"

void matrix_copy_3x3(float src[][3], float dest[][3]) {
	int i, j;
	for(i = 0;i < 3;i++) {
		for(j = 0;j < 3;j++)
			dest[i][j] = src[i][j];
	}
}

void matrix_multiply_3x3(float a[][3], float b[][3], float dest[][3]) {
	int i, j, k;
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			dest[i][j] = 0;
			for(k = 0; k < 3; k++) {
				dest[i][j] +=  a[i][k] *  b[k][j];
			}
		}
	}
}

