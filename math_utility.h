#ifndef MATH_UTILITY
#define MATH_UTILITY

#define M_PI  3.14159265358979323846
#define MATRIX_SIZE 3

double max( double value1, double value2 );
double min( double value1, double value2 );
double sqr( double v );
double degrees_to_radians( double value );
double magnitude( double* input_vector );
double distance_between(double* vector1, double* vector2 );
double dot_product( double* v1, double* v2 );
void normalize( double* vector );
double simplify( double input );
double clamp( double input );

void vector_add( double* input, double num );
void vector_mult( double* input, double num );

void vect_degrees_to_radians( double* input );

void apply_xyz_rotation( double* input, double* direction );

#endif