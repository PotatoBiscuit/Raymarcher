#include <math.h>
#include "math_utility.h"

double max( double value1, double value2 ) {
	if( value1 > value2 ){
		return value1;
	}
	return value2;
}

double min( double value1, double value2 ) {
	if( value1 < value2 ){
		return value1;
	}
	return value2;
}

double sqr(double v) {	//Return the square of the number passed in
  return v*v;
}

double degrees_to_radians(double value){	//Converts input from degrees to radians
	return 2*M_PI*value/360;
}

double magnitude(double* input_vector){	//Calculate the magnitude/distance of the 3D input vector
	return sqrt(sqr(input_vector[0]) + sqr(input_vector[1]) + sqr(input_vector[2]));
}

double magnitude_2D(double* input_vector){	//Calculate the magnitude/distance of the 2D input vector
	return sqrt(sqr(input_vector[0]) + sqr(input_vector[1]));
}

double distance_between(double* x_array, double* y_array ){
	return sqrt(sqr(x_array[0] - y_array[0]) + sqr(x_array[1] - y_array[1]) + sqr(x_array[2] - y_array[2]));
}

double dot_product( double* x_array, double* y_array ){
	return x_array[0]*y_array[0] + x_array[1]*y_array[1] + x_array[2]*y_array[2];
}

void normalize(double* vector) {
    double length = magnitude(vector);
    vector[0] /= length;
    vector[1] /= length;
    vector[2] /= length;
}

double simplify(double input){	//Simplify number to the thousandth decimal place
	return round(input*1000)/1000;
}

double clamp(double input){		//Return 1 if the input is above one, and return 0 if the input is negative
	if(input > 1) return 1;
	if(input < 0) return 0;
	return input;
}

void vector_add( double* input, double num ){
	input[0] += num;
	input[1] += num;
	input[2] += num;
}

void vector_mult( double* input, double num ){
	input[0] *= num;
	input[1] *= num;
	input[2] *= num;
}

void vector_mult_sp( double* input, double num, double* result ){
	result[0] = input[0] * num;
	result[1] = input[1] * num;
	result[2] = input[2] * num;
}

void reflect( double* ray, double* normal, double* result ){
	double dot_ray_norm = dot_product( ray, normal );
	result[0] = ray[0] - 2 * dot_ray_norm * normal[0];
	result[1] = ray[1] - 2 * dot_ray_norm * normal[1];
	result[2] = ray[2] - 2 * dot_ray_norm * normal[2];
}

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

void vect_degrees_to_radians( double* input ){
	input[0] = degrees_to_radians(input[0]);
	input[1] = degrees_to_radians(input[1]);
	input[2] = degrees_to_radians(input[2]);
}

void apply_xyz_rotation( double* input, double* direction ){
	double x_axis[3] = {1.0, 0.0, 0.0};
	double y_axis[3] = {0.0, 1.0, 0.0};
	double z_axis[3] = {0.0, 0.0, 1.0};

	if( direction[0] != 0.0 ){
		apply_rotation( input, x_axis, direction[0] );
	}

	if( direction[1] != 0.0 ){
		apply_rotation( input, y_axis, direction[1] );
	}

	if( direction[2] != 0.0 ){
		apply_rotation( input, z_axis, direction[2] );
	}
}