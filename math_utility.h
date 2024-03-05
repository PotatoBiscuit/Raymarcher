#ifndef MATH_UTILITY
#define MATH_UTILITY

#define M_PI  3.14159265358979323846

double sqr(double v);
double degrees_to_radians(double value);
double calculate_distance(double* input_vector);
void normalize(double* vector);
double simplify(double input);
double clamp(double input);

#endif