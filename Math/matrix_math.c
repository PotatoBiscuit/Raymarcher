#include <math.h>
#include "vector_math.h"
#include "matrix_math.h"

void matrix_mult( double input[][3], double num, double result[][3] ){
	vector_mult_sp( input[0], num, result[0] );
	vector_mult_sp( input[1], num, result[1] );
	vector_mult_sp( input[2], num, result[2] );
}

void matrix_cross_mult( double a[][3], double b[][3], double result[][3] ){
	for( int i = 0; i < MATRIX_SIZE; i++ ){
		for( int j = 0; j < MATRIX_SIZE; j++ ){
			result[i][j] = 0;
			for( int k = 0; k < MATRIX_SIZE; k++ ){
				result[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

void matrix_cross_mult_sp( double* a, double b[][3] ){
	double temp_result[3] = {0.0};
	for( int i = 0; i < MATRIX_SIZE; i++ ){
		for( int k = 0; k < MATRIX_SIZE; k++ ){
			temp_result[i] += a[k] * b[k][i];
		}
	}

	for( int i = 0; i < MATRIX_SIZE; i++ ){
		a[i] = temp_result[i];
	}
}

void add_matrices( double a[][3], double b[][3] ){
	for( int i = 0; i < MATRIX_SIZE; i++ ){
		for( int j = 0; j < MATRIX_SIZE; j++ ){
			a[i][j] += b[i][j];
		}
	}
}

void get_rotation_matrix_X( double matrix[][3], double theta ){
	matrix[0][0] = 1;
	matrix[0][1] = 0;
	matrix[0][2] = 0;

	matrix[1][0] = 0;
	matrix[1][1] = cos( theta );
	matrix[1][2] = -sin( theta );

	matrix[2][0] = 0;
	matrix[2][1] = sin( theta );
	matrix[2][2] = cos( theta );
}

void get_rotation_matrix_Y( double matrix[][3], double theta ){
	matrix[0][0] = cos( theta );
	matrix[0][1] = 0;
	matrix[0][2] = sin( theta );

	matrix[1][0] = 0;
	matrix[1][1] = 1;
	matrix[1][2] = 0;

	matrix[2][0] = -sin( theta );
	matrix[2][1] = 0;
	matrix[2][2] = cos( theta );
}

void get_rotation_matrix_Z( double matrix[][3], double theta ){
	matrix[0][0] = cos( theta );
	matrix[0][1] = -sin( theta );
	matrix[0][2] = 0;

	matrix[1][0] = sin( theta );
	matrix[1][1] = cos( theta );
	matrix[1][2] = 0;

	matrix[2][0] = 0;
	matrix[2][1] = 0;
	matrix[2][2] = 1;
}

void apply_xyz_rotation( double* input, double* direction ){
	double x_axis[3] = {1.0, 0.0, 0.0};
	double y_axis[3] = {0.0, 1.0, 0.0};
	double z_axis[3] = {0.0, 0.0, 1.0};

	double rotation_matrix[3][3];
	if( direction[0] != 0.0 ){
		get_rotation_matrix_X(rotation_matrix, direction[0] );
		matrix_cross_mult_sp( input, rotation_matrix );
	}

	if( direction[1] != 0.0 ){
		get_rotation_matrix_Y(rotation_matrix, direction[1] );
		matrix_cross_mult_sp( input, rotation_matrix );
	}

	if( direction[2] != 0.0 ){
		get_rotation_matrix_Z(rotation_matrix, direction[2] );
		matrix_cross_mult_sp( input, rotation_matrix );
	}
}

// Rodrigues/Euler's matrix rotations are very cool, but slow, the below are unused for now
void get_rotation_matrix( double matrix[][3], double* rotation_axis, double theta ){
	double K_matrix[3][3];
	K_matrix[0][0] = 0.0;
	K_matrix[0][1] = -rotation_axis[2];
	K_matrix[0][2] = rotation_axis[1];
	K_matrix[1][0] = rotation_axis[2];
	K_matrix[1][1] = 0.0;
	K_matrix[1][2] = -rotation_axis[0];
	K_matrix[2][0] = -rotation_axis[1];
	K_matrix[2][1] = rotation_axis[0];
	K_matrix[2][2] = 0.0;

	double identity[3][3] = {0.0};
	identity[0][0] = 1;
	identity[1][1] = 1;
	identity[2][2] = 1;

	//add I
	add_matrices( matrix, identity );

	//add sin(theta)K
	double K_sin[3][3] = {0.0};
	matrix_mult( K_matrix, sin(theta), K_sin );
	add_matrices( matrix, K_sin );

	//add (1-cos(theta))K^2
	double K_cross[3][3] = {0.0};
	matrix_cross_mult( K_matrix, K_matrix, K_cross );
	double K_cross_cos[3][3] = {0.0};
	matrix_mult( K_cross, (1.0 - cos(theta)), K_cross_cos );
	add_matrices( matrix, K_cross_cos );
}

void apply_rotation( double* input, double* rotation_axis, double theta ){
	double rotation_matrix[3][3] = {0.0};
	get_rotation_matrix( rotation_matrix, rotation_axis, theta );
	matrix_cross_mult_sp( input, rotation_matrix );
}