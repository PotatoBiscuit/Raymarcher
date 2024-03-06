#ifndef RAYMARCH
#define RAYMARCH

typedef struct{	//Holds object intersection information
	int best_index;
	double best_t;
} Tuple;

#define MAX_RECURSION 7
#define INTERSECTION_LIMIT .0001
#define OUTER_BOUNDS 1000000

#endif