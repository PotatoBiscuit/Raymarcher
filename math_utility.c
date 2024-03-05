#include <math.h>
#include "math_utility.h"

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

double magnitude(double* input_vector){	//Calculate the magnitude/distance of the input vector
	return sqrt(sqr(input_vector[0]) + sqr(input_vector[1]) + sqr(input_vector[2]));
}

double distance_between(double* vector1, double* vector2 ){
	return sqrt(sqr(vector1[0] - vector2[0]) + sqr(vector1[1] - vector2[1]) + sqr(vector1[2] - vector2[2]));
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