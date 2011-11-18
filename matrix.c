#include "matrix.h"

void matrix_3_3_multiply(float a[][3], float b[][3], float result[][3]) {
	int i, j, k;
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			result[i][j] = 0;
			for(k = 0; k < 3; k++) {
				result[i][j] +=  a[i][k] *  b[k][j];
			}
		}
	}
}

void matrix_3_3_copy(float src[][3], float dest[][3]) {
	int i, j;
	for(i = 0;i < 3;i++) {
		for(j = 0;j < 3;j++)
			dest[i][j] = src[i][j];
	}
}

