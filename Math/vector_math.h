#ifndef VECTOR_MATH
#define VECTOR_MATH

double magnitude( double* input_vector );
double magnitude_2D( double* input_vector );
double distance_between(double* vector1, double* vector2 );
double dot_product( double* v1, double* v2 );
void normalize( double* vector );

void vector_add( double* input, double num );
void vector_mult( double* input, double num );
void vector_mult_sp( double* input, double num, double* result );

void reflect( double* ray, double* normal, double* result );

void vect_degrees_to_radians( double* input );

#endif