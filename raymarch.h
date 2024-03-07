#ifndef RAYMARCH
#define RAYMARCH

typedef struct{	//Holds object intersection information
	int best_index;
	double best_t;
    double position[3];
} Intersect;

#define MAX_RECURSION 7
#define INTERSECTION_LIMIT .000001
#define OUTER_BOUNDS 1000000
#define COLOR_LIMIT 256.0
#define MAX_STEPS 1000

#endif