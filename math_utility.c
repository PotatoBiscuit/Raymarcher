#include <math.h>
#include "math_utility.h"

double degrees_to_radians(double value){	//Converts input from degrees to radians
	return 2*M_PI*value/360;
}

void normalize(double* vector) {
    double length = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
    vector[0] /= length;
    vector[1] /= length;
    vector[2] /= length;
}