#include <math.h>
#include "math_utility.h"

double sqr(double v) {	//Return the square of the number passed in
  return v*v;
}

double degrees_to_radians(double value){	//Converts input from degrees to radians
	return 2*M_PI*value/360;
}

double calculate_distance(double* input_vector){	//Calculate the magnitude/distance of the input vector
	return sqrt(sqr(input_vector[0]) + sqr(input_vector[1]) + sqr(input_vector[2]));
}

void normalize(double* vector) {
    double length = calculate_distance(vector);
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