#include <math.h>
#include "simple_math.h"
#include "vector_math.h"

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

void vect_degrees_to_radians( double* input ){
	input[0] = degrees_to_radians(input[0]);
	input[1] = degrees_to_radians(input[1]);
	input[2] = degrees_to_radians(input[2]);
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