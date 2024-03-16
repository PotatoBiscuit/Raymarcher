#include <math.h>
#include "simple_math.h"

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

double simplify(double input){	//Simplify number to the thousandth decimal place
	return round(input*1000)/1000;
}

double clamp(double input){		//Return 1 if the input is above one, and return 0 if the input is negative
	if(input > 1) return 1;
	if(input < 0) return 0;
	return input;
}


