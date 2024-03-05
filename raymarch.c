#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "math_utility.h"
#include "parse_json.h"

typedef struct{	//Holds object intersection information
	int best_index;
	double best_t;
} Tuple;

#define MAX_RECURSION 7

void argument_checker(int c, char** argv){	//Check input arguments for validity
	int i = 0;
	int j = 0;
	char* periodPointer;
	if(c != 5){	//Ensure that five arguments are passed in through command line
		fprintf(stderr, "Error: Incorrect amount of arguments\n");
		exit(1);
	}
	
	while(1){	//Ensure that both the width and height arguments are numbers
		if(*(argv[1] + i) == '\0' && *(argv[2] + j) == '\0'){
			break;
		}
		else if(*(argv[1] + i) == '\0'){
			i--;
		}
		else if(*(argv[2] + j) == '\0'){
			j--;
		}
		
		if(!isdigit(*(argv[1] + i)) || !isdigit(*(argv[2] + j))){
			fprintf(stderr, "Error: Width or Height field is not a number\n");
			exit(1);
		}
		i++;
		j++;
	}
	
	periodPointer = strrchr(argv[3], '.');	//Ensure that the input scene file has an extension .json
	if(periodPointer == NULL){
		fprintf(stderr, "Error: Input scene file does not have a file extension\n");
		exit(1);
	}
	if(strcmp(periodPointer, ".json") != 0){
		fprintf(stderr, "Error: Input scene file is not of type JSON\n");
		exit(1);
	}
	
	periodPointer = strrchr(argv[4], '.');	//Ensure that the output picture file has an extension .ppm
	if(periodPointer == NULL){
		fprintf(stderr, "Error: Output picture file does not have a file extension\n");
		exit(1);
	}
	if(strcmp(periodPointer, ".ppm") != 0){
		fprintf(stderr, "Error: Output picture file is not of type PPM\n");
		exit(1);
	}
}

double sphere_intersection(double* Ro, double* Rd, double* C, double radius){ //Calculates the solutions to a sphere intersection
	//Sphere equation is x^2 + y^2 + z^2 = r^2
	//Parameterize: (x-Cx)^2 + (y-Cy)^2 + (z-Cz)^2 - r^2 = 0
	//Substitute with ray:
	//(Rox + t*Rdx - Cx)^2 + (Roy + t*Rdy - Cy)^2 + (Roz + t*Rdz - Cz)^2 - r^2 = 0
	//Solve for t:
	//a = Rdx^2 + Rdy^2 + Rdz^2
	double a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
	//b = (2RdxRox - 2RdxCx) + (2RdyRoy - 2RdyCy) + (2RdzRoz - 2RdzCz)
	double b = (2*Rd[0]*Ro[0] - 2*Rd[0]*C[0]) + (2*Rd[1]*Ro[1] - 2*Rd[1]*C[1]) + (2*Rd[2]*Ro[2] - 2*Rd[2]*C[2]);
	//c = (Rox^2 - 2RoxCx + Cx^2) + (Roy^2 - 2RoyCy + Cy^2) + (Roz^2 - 2RozCz + Cz^2) - r^2
	double c = (sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[0])) + (sqr(Ro[1]) - 2*Ro[1]*C[1] + sqr(C[1])) + (sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2])) - sqr(radius);
	
	double t0;
	double t1;
	double det = sqr(b) - 4*a*c;
	if(det < 0) return 0;	//If there are no real solutions return 0
	
	t0 = (-b - sqrt(det))/(2*a);	//Calculate both solutions
	t1 = (-b + sqrt(det))/(2*a);
	if(t0 <= 0 && t1 <= 0) return 0; //If both solutions are less than 0, return 0
	if(t0 <= 0 && t1 > 0) return t1; //If only t1 is greater than 0, return t1
	if(t1 <= 0 && t0 > 0) return t0; //If only t0 is greater than 0, return t0
	if(t0 > t1) return t1;	//If t0 is greater than t1, return t1
	if(t1 > t0) return t0;	//If t1 is greater than t0, return t0
	
	return t0;	//If both solutions are equal, just return t0
	
}

double special_sphere_intersection(double* Ro, double* Rd, double* C, double radius){ //Calculates the solutions to a sphere intersection
	//Sphere equation is x^2 + y^2 + z^2 = r^2
	//Parameterize: (x-Cx)^2 + (y-Cy)^2 + (z-Cz)^2 - r^2 = 0
	//Substitute with ray:
	//(Rox + t*Rdx - Cx)^2 + (Roy + t*Rdy - Cy)^2 + (Roz + t*Rdz - Cz)^2 - r^2 = 0
	//Solve for t:
	//a = Rdx^2 + Rdy^2 + Rdz^2
	double a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
	//b = (2RdxRox - 2RdxCx) + (2RdyRoy - 2RdyCy) + (2RdzRoz - 2RdzCz)
	double b = (2*Rd[0]*Ro[0] - 2*Rd[0]*C[0]) + (2*Rd[1]*Ro[1] - 2*Rd[1]*C[1]) + (2*Rd[2]*Ro[2] - 2*Rd[2]*C[2]);
	//c = (Rox^2 - 2RoxCx + Cx^2) + (Roy^2 - 2RoyCy + Cy^2) + (Roz^2 - 2RozCz + Cz^2) - r^2
	double c = (sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[0])) + (sqr(Ro[1]) - 2*Ro[1]*C[1] + sqr(C[1])) + (sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2])) - sqr(radius);
	
	double t0;
	double t1;
	double det = sqr(b) - 4*a*c;
	if(det < 0) return 0;	//If there are no real solutions return 0
	
	t0 = (-b - sqrt(det))/(2*a);	//Calculate both solutions
	t1 = (-b + sqrt(det))/(2*a);
	if(t0 <= 0 && t1 <= 0) return 0; //If both solutions are less than 0, return 0
	if(t0 <= 0 && t1 > 0) return t1; //If only t1 is greater than 0, return t1
	if(t1 <= 0 && t0 > 0) return t0; //If only t0 is greater than 0, return t0
	if(t0 > t1) return t0;	//If t0 is greater than t1, return t1
	if(t1 > t0) return t1;	//If t1 is greater than t0, return t0
	
	return t0;	//If both solutions are equal, just return t0
	
}

double plane_intersection(double* Ro, double* Rd, double* C, double* N){ //Calculates the solution of a plane intersection
	//Solve for Plane Equation:
	//Nx(x - Cx) + Ny(y - Cy) + Nz(z - Cz) = 0
	//Plug in our Ray:
	//Nx(Rox + t*Rdx - Cx) + Ny(Roy + t*Rdy - Cy) + Nz(Roz + t*Rdz - Cz) = 0
	//Solve for t:
	//t = ((NxCx - NxRox) + (NyCy - NyRoy) + (NzCz - NzRoz))/(RdxNx + RdyNy + RdzNz)
	double t = ((N[0]*C[0] - N[0]*Ro[0]) + (N[1]*C[1] - N[1]*Ro[1]) + (N[2]*C[2] - N[2]*Ro[2]))/(Rd[0]*N[0] + Rd[1]*N[1] + Rd[2]*N[2]);
	if(t > 0) return t;	//Return solution if it is greater than 0
	return 0;	//else just return 0
}

double fang(double a0, double theta, double* vO, double* vL){	//Return angular attenuation value
	//vO is vector pointing from the light to the object
	//vL is the direction of the light
	//theta and a0 have to do with spotlight width and drop-off
	double cos_phi = vO[0]*vL[0] + vO[1]*vL[1] + vO[2]*vL[2];
	if(theta == 0 || a0 == 0) return 1;	//If we aren't using spotlights, we can just return 1
	if(cos_phi < cos(theta)){
		return 0;
	}
	return pow(cos_phi, a0);
}

double frad(double a0, double a1, double a2, double distance){	//Return radial attenuation value
	if(distance == 0) return 1;		//If the light is somehow on the object, return 1
	double denominator = (a0 + a1*distance + a2*sqr(distance));	//Calculate denominator of radial attenuation fnc
	if (denominator == 0) return 1;	//If denominator is a faulty value of 0, return 1
	return 1/denominator;	//If everything goes smoothly, return the real radial attenuation value
}

double* diffuse(double* L, double* N, double* Cd, double* Ci){	//Return diffuse color value
	//Cd is diffuse color, and Ci is light color
	double* diffused_color = malloc(sizeof(double)*3);
	//Calculate diffuse color values
	double dot_product_L_N = L[0] * N[0] + L[1] * N[1] + L[2] * N[2];
	diffused_color[0] = (dot_product_L_N)*Cd[0]*Ci[0];
	diffused_color[1] = (dot_product_L_N)*Cd[1]*Ci[1];
	diffused_color[2] = (dot_product_L_N)*Cd[2]*Ci[2];
	if(diffused_color[0] < 0) diffused_color[0] = 0;	//Diffuse colors cannot be negative
	if(diffused_color[1] < 0) diffused_color[1] = 0;
	if(diffused_color[2] < 0) diffused_color[2] = 0;
	return diffused_color;		//Return diffuse color
}

double* specular(double* R, double* V, double* Cs, double* Ci, double* N, double* L){	//Return specular color value
	double* speculared_color = malloc(sizeof(double)*3);
	double dot_product_R_V = R[0]*V[0] + R[1]*V[1] + R[2]*V[2];
	double dot_product_N_L = N[0]*L[0] + N[1]*L[1] + N[2]*L[2];
	if(dot_product_N_L <= 0 || dot_product_R_V <= 0){
		//If our object normal and vector from the light to object dot product is 0 or negative,
		//or if the dot product of our reflected and camera vector are 0
		//return speculared_colors of {0,0,0}
		speculared_color[0] = 0;
		speculared_color[1] = 0;
		speculared_color[2] = 0;
		return speculared_color;
	}
	//Calculate specular color, with ns value hardcoded to 20
	speculared_color[0] = pow(dot_product_R_V, 20)*Cs[0]*Ci[0];
	speculared_color[1] = pow(dot_product_R_V, 20)*Cs[1]*Ci[1];
	speculared_color[2] = pow(dot_product_R_V, 20)*Cs[2]*Ci[2];
	if(speculared_color[0] < 0) speculared_color[0] = 0;	//Specular color may not be negative
	if(speculared_color[1] < 0) speculared_color[1] = 0;
	if(speculared_color[2] < 0) speculared_color[2] = 0;
	return speculared_color;	//Return specular color
}

double* reflect(double* L, double* N){	//Reflect vector L across a normal N
	double* reflect_vector = malloc(sizeof(double)*3);
	double dot_product_L_N = (L[0] * N[0]) + (L[1] * N[1]) + (L[2] * N[2]);
	reflect_vector[0] = L[0] - 2*dot_product_L_N * N[0];
	reflect_vector[1] = L[1] - 2*dot_product_L_N * N[1];
	reflect_vector[2] = L[2] - 2*dot_product_L_N * N[2];
	return reflect_vector;
}

double* refract(double* Rd, double* N, double ior){  //Use Snell's Law to find refracted ray
	double* refract_vector = malloc(sizeof(double)*3);
	double refract_sin;
	double refract_cos;
	double cross_product_distance;
	double a1[3];
	double a[3];
	double b[3];
	a1[0] = N[1]*Rd[2] - N[2]*Rd[1];
	a1[1] = -(N[0]*Rd[2] - N[2]*Rd[0]);
	a1[2] = N[0]*Rd[1] - N[1]*Rd[0];
	cross_product_distance = calculate_distance(a1);
	a[0] = a1[0]/cross_product_distance;
	a[1] = a1[1]/cross_product_distance;
	a[2] = a1[2]/cross_product_distance;
	b[0] = a[1]*N[2] - a[2]*N[1];
	b[1] = -(a[0]*N[2] - a[2]*N[0]);
	b[2] = a[0]*N[1] - a[1]*N[0];
	refract_sin = (Rd[0]*b[0] + Rd[1]*b[1] + Rd[2]*b[2])/ior;
	refract_cos = sqrt(1 - sqr(refract_sin));
	refract_vector[0] = -N[0]*refract_cos + b[0]*refract_sin;
	refract_vector[1] = -N[1]*refract_cos + b[1]*refract_sin;
	refract_vector[2] = -N[2]*refract_cos + b[2]*refract_sin;
	normalize(refract_vector);
	
	return refract_vector;
}

Tuple* shoot(Object** object_array, int object_counter, double* Ro, double* Rd){	//Find object intersections
	Tuple* intersection = malloc(sizeof(Tuple));
	int parse_count = 1;
	double best_t = INFINITY;
	int best_index = -1;
	double t = 0;
	
	while(parse_count < object_counter + 1){	//Iterate through object array and test for intersections
		if(object_array[parse_count]->kind == 1){	//If sphere, test for sphere intersections
			t = sphere_intersection(Ro, Rd, object_array[parse_count]->sphere.position,
									object_array[parse_count]->sphere.radius);
		}else if(object_array[parse_count]->kind == 2){	//If plane, test for a plane intersection
			t = plane_intersection(Ro, Rd, object_array[parse_count]->plane.position,
									object_array[parse_count]->plane.normal);
		}else{
			parse_count++;
			continue;
		}
		
		if(t < best_t && t > .0001){	//Store object index with the closest intersection
			best_t = t;					//Store distance to closest intersection
			best_index = parse_count;
		}
		parse_count++;
	}
	intersection->best_index = best_index;
	intersection->best_t = best_t;
	return intersection;
}

//Forward declaration of render_light for the functions get_reflect_color() and get_refract_color()
double* render_light(Object**, int, double, int, double*, double*, int);

double* get_reflect_color(Object** object_array, int object_counter, int best_index,  //Calculate object reflections
							double* Ron, double* Rd, double* N, int layer){
	double* reflected_color;
	double* R1;
	Tuple* intersection;
	R1 = reflect(Rd, N);	//Reflect ray coming from camera to find reflection
	normalize(R1);
	
	intersection = shoot(object_array, object_counter, Ron, R1);	//Find intersection of this reflected ray
	if(intersection->best_t > 0 && intersection->best_t != INFINITY){	//If the intersection is valid, calculate reflected light
		reflected_color = render_light(object_array, object_counter, intersection->best_t,
										intersection->best_index, Ron, R1, layer + 1);
		if(object_array[best_index]->kind == 1){
			reflected_color[0] = reflected_color[0]*object_array[best_index]->sphere.reflectivity;
			reflected_color[1] = reflected_color[1]*object_array[best_index]->sphere.reflectivity;
			reflected_color[2] = reflected_color[2]*object_array[best_index]->sphere.reflectivity;
		}
		else if(object_array[best_index]->kind == 2){
			reflected_color[0] = reflected_color[0]*object_array[best_index]->plane.reflectivity;
			reflected_color[1] = reflected_color[1]*object_array[best_index]->plane.reflectivity;
			reflected_color[2] = reflected_color[2]*object_array[best_index]->plane.reflectivity;
		}
	}else{	//If no intersection found, return black
		reflected_color = malloc(sizeof(double)*3);
		reflected_color[0] = 0;
		reflected_color[1] = 0;
		reflected_color[2] = 0;
	}
	free(intersection);
	free(R1);
	return reflected_color;
}

double* get_refract_color(Object** object_array, int object_counter, int best_index,  //Calculate object refraction
							double* Ron, double* Rd, double* N, int layer){
	double Ron1[3];
	double N1[3];
	double* refracted_vector;
	double* refracted_vector1;
	double* refracted_color = NULL;
	Tuple* intersection;
	double t = 0;
	if(object_array[best_index]->kind == 1){//If the object is a sphere, two refractions must be performed
		refracted_vector1 = refract(Rd, N, object_array[best_index]->sphere.ior);	//Calculate first refraction
		//Find next sphere intersection with refracted vector
		t = special_sphere_intersection(Ron, refracted_vector1, object_array[best_index]->sphere.position, object_array[best_index]->sphere.radius);
		if(t <= .0001 || t == INFINITY){	//If no intersection found, just use our current vector as final refracted vector
			refracted_vector = refracted_vector1;
		}else{	//If interesection is found, calculate a new refracted vector with our previous refracted vector
			Ron1[0] = Ron[0] + refracted_vector1[0]*t;
			Ron1[1] = Ron[1] + refracted_vector1[1]*t;
			Ron1[2] = Ron[2] + refracted_vector1[2]*t;
			N1[0] = object_array[best_index]->sphere.position[0] - Ron1[0];
			N1[1] = object_array[best_index]->sphere.position[1] - Ron1[1];
			N1[2] = object_array[best_index]->sphere.position[2] - Ron1[2];
			normalize(N1);
			refracted_vector = refract(refracted_vector1, N1, object_array[best_index]->sphere.ior);
			free(refracted_vector1);
		}
		
		//Find closest object intersection with our new final refracted vector
		intersection = shoot(object_array, object_counter, Ron1, refracted_vector);
		if(intersection->best_t > 0 && intersection->best_t != INFINITY){	//If valid intersection found, calculate refracted color
			refracted_color = render_light(object_array, object_counter, intersection->best_t,
											intersection->best_index, Ron1, refracted_vector, layer+1);
			refracted_color[0] = refracted_color[0]*object_array[best_index]->sphere.refractivity;
			refracted_color[1] = refracted_color[1]*object_array[best_index]->sphere.refractivity;
			refracted_color[2] = refracted_color[2]*object_array[best_index]->sphere.refractivity;
		}
		free(intersection);
		free(refracted_vector);
	}
	else if(object_array[best_index]->kind == 2){	//If object is a plane, we need to calculate for refraction only once
		refracted_vector = refract(Rd, N, object_array[best_index]->plane.ior);
		
		//Find object intersection with our refracted vector
		intersection = shoot(object_array, object_counter, Ron, refracted_vector);
		if(intersection->best_t > 0 && intersection->best_t != INFINITY){	//If intersection is valid, calculate refracted color
			refracted_color = render_light(object_array, object_counter, intersection->best_t,
											intersection->best_index, Ron, refracted_vector, layer+1);
			refracted_color[0] = refracted_color[0]*object_array[best_index]->plane.refractivity;
			refracted_color[1] = refracted_color[1]*object_array[best_index]->plane.refractivity;
			refracted_color[2] = refracted_color[2]*object_array[best_index]->plane.refractivity;
		}
		free(intersection);
		free(refracted_vector);
	}
	
	
	if(refracted_color == NULL){	//If no refracted intersections are found, return black
		refracted_color = malloc(sizeof(double)*3);
		refracted_color[0] = 0;
		refracted_color[1] = 0;
		refracted_color[2] = 0;
	}
	
	return refracted_color;
}

//Calculate color values using lights
double* render_light(Object** object_array, int object_counter, double best_t,
						int best_index, double* Ro, double* Rd, int layer){
	double t = 0;
	int parse_count = 1;
	int parse_count1 = 1;
	double Ron[3];
	double Rdn[3];
	double* color = malloc(sizeof(double)*3);
	double* reflected_color;
	double* refracted_color;
	double* diffused_color;
	double* speculared_color;
	double N[3];
	double L[3];
	double* R;
	double V[3];
	double distance_from_light;
	Tuple* intersection;
	double portion_not_refracted_reflected = 0;
	double radial_attenuation;
	double angular_attenuation;
	
	
	Ron[0] = best_t * Rd[0] + Ro[0];	//Calculate the intersection point of the object we hit
	Ron[1] = best_t * Rd[1] + Ro[1];
	Ron[2] = best_t * Rd[2] + Ro[2];
			
	parse_count = 1;
	parse_count1 = 1;
	
	color[0] = 0;	//Set color value to black (for now)
	color[1] = 0;
	color[2] = 0;
	
	if(layer > MAX_RECURSION){	//Exit function if we have recursed too far
		return color;
	}
	
	//Calculate object normals, as well as portions of color dedicated to reflection and refraction
	if(object_array[best_index]->kind == 1){
		N[0] = Ron[0] - object_array[best_index]->sphere.position[0];
		N[1] = Ron[1] - object_array[best_index]->sphere.position[1];
		N[2] = Ron[2] - object_array[best_index]->sphere.position[2];
		portion_not_refracted_reflected = 1 - object_array[best_index]->sphere.reflectivity -
											object_array[best_index]->sphere.refractivity;
	}
	else if(object_array[best_index]->kind == 2){
		N[0] = object_array[best_index]->plane.normal[0];
		N[1] = object_array[best_index]->plane.normal[1];
		N[2] = object_array[best_index]->plane.normal[2];
		portion_not_refracted_reflected = 1 - object_array[best_index]->plane.reflectivity -
											object_array[best_index]->plane.refractivity;
	}
	normalize(N);
	
	//Calculate reflection and refraction color values, add them to color total
	reflected_color = get_reflect_color(object_array, object_counter, best_index, Ron, Rd, N, layer);
	refracted_color = get_refract_color(object_array, object_counter, best_index, Ron, Rd, N, layer);
	color[0] += reflected_color[0] + refracted_color[0];
	color[1] += reflected_color[1] + refracted_color[1];
	color[2] += reflected_color[2] + refracted_color[2];
	
	free(reflected_color);
	free(refracted_color);
	
	while(parse_count < object_counter + 1){	//Iterate through object array and check for lights
		if(object_array[parse_count]->kind == 3){
			//Create vector pointing to light source, originating from our intersection
			Rdn[0] = object_array[parse_count]->light.position[0] - Ron[0];
			Rdn[1] = object_array[parse_count]->light.position[1] - Ron[1];
			Rdn[2] = object_array[parse_count]->light.position[2] - Ron[2];
			distance_from_light = calculate_distance(Rdn);	//Calculate distance from light to intersection
			normalize(Rdn);	//normalize our object to light vector
			
			while(parse_count1 < object_counter + 1){	//Check to see if our point of intersection is in shadow
				if(parse_count1 == best_index){	//The object we intersected cannot overshadow itself!
					parse_count1++;
					continue;
				}
				if(object_array[parse_count1]->kind == 1){	//See if a sphere overshadows our point of intersection
					t = sphere_intersection(Ron, Rdn, object_array[parse_count1]->sphere.position,
									object_array[parse_count1]->sphere.radius);			
					parse_count1++;
				}else if(object_array[parse_count1]->kind == 2){ //See if a plane overshadows our point of intersection
					t = plane_intersection(Ron, Rdn, object_array[parse_count1]->plane.position,
											object_array[parse_count1]->plane.normal);
					parse_count1++;
				}else{	//If a light was found, skip it
					parse_count1++;
					continue;
				}
				
				if(t > 0 && t < distance_from_light){	//If a valid overshadowing object was found, break
					break;
				}else if(t > 0 && t > distance_from_light){ //If object found behind light, it does not cast a shadow
					t = 0;
				}
			}
			
			L[0] = Rdn[0];	//Store object to light vector into L
			L[1] = Rdn[1];
			L[2] = Rdn[2];
			
			V[0] = Rd[0];	//Store vector pointing from camera to object
			V[1] = Rd[1];
			V[2] = Rd[2];
			
			if(t <= 0){
				if(object_array[best_index]->kind == 1){
					normalize(N);
					R = reflect(L, N);	//Get reflected vector of L
					
					//Calculate diffuse and specular color
					diffused_color = diffuse(L, N, object_array[best_index]->sphere.diffuse_color,
												object_array[parse_count]->light.color);
					speculared_color = specular(R, V, object_array[best_index]->sphere.specular_color,
												object_array[parse_count]->light.color, N, L);
					
				}else if(object_array[best_index]->kind == 2){
					
					
					R = reflect(L, N);  //Get reflected vector of L
					
					//Calculate diffuse and specular color
					diffused_color = diffuse(L, N, object_array[best_index]->plane.diffuse_color,
												object_array[parse_count]->light.color);
					speculared_color = specular(R, V, object_array[best_index]->plane.specular_color,
												object_array[parse_count]->light.color, N, L);
					
				}
				else{	//If the current object is somehow a light
					fprintf(stderr,"Error: Tried to render light as a shape primitive");
					exit(1);
				}
				
				
				//Reverse direction of Rdn to be used in angular attenuation calculations
				Rdn[0] = -Rdn[0];
				Rdn[1] = -Rdn[1];
				Rdn[2] = -Rdn[2];
				//Add total light values together
				radial_attenuation = frad(object_array[parse_count]->light.radial_a0,
								object_array[parse_count]->light.radial_a1,
								object_array[parse_count]->light.radial_a2, distance_from_light);
				angular_attenuation = fang(object_array[parse_count]->light.angular_a0,
								object_array[parse_count]->light.theta, Rdn,
								object_array[parse_count]->light.direction);
								
				color[0] += 	portion_not_refracted_reflected *
								radial_attenuation *
								angular_attenuation *
								(diffused_color[0] + speculared_color[0]);
								
				color[1] += 	portion_not_refracted_reflected *
								radial_attenuation *
								angular_attenuation *
								(diffused_color[1] + speculared_color[1]);
								
				color[2] += 	portion_not_refracted_reflected *
								radial_attenuation *
								angular_attenuation *
								(diffused_color[2] + speculared_color[2]);
				
				free(diffused_color);	//free memory
				free(speculared_color);
				free(R);
			}
			t = 0;
			parse_count1 = 1;
		}
		parse_count++;
	}
	//Clamp color values
	color[0] = clamp(color[0]);
	color[1] = clamp(color[1]);
	color[2] = clamp(color[2]);
	return color;
}

void raycast_scene(Object** object_array, int object_counter, double** pixel_buffer, int N, int M){	//This raycasts our object_array
	int parse_count = 0;
	int pixel_count = 0;
	int i;
	double Ro[3];
	double Rd[3];
	double* color;
	double cx = 0;
	double cy = 0;
	double w;
	double h;
	double pixwidth;
	double pixheight;
	Tuple* intersection;
	
	if(object_array[parse_count]->kind != 0){	//If camera is not present, throw an error
		fprintf(stderr, "Error: You must have one object of type camera\n");
		exit(1);
	}
	
	//Grab camera width and height, and calculate our pixel widths and pixel heights
	w = object_array[parse_count]->camera.width;
	pixwidth = w/N;
	h = object_array[parse_count]->camera.height;
	pixheight = h/M;
	parse_count++;
	
	//Create origin point for our vector
	Ro[0] = 0;
	Ro[1] = 0;
	Ro[2] = 0;
	
	for(int y = 0; y < M; y += 1){	//Raycast every shape for each pixel
		for(int x = 0; x < N; x += 1){
			Rd[0] = cx - (w/2) + pixwidth * (x + .5);	//Create direction vector
			Rd[1] = cy - (h/2) + pixheight * (y + .5);
			Rd[2] = 1;
			normalize(Rd);
			intersection = shoot(object_array, object_counter, Ro, Rd);

			
			if(intersection->best_t > 0 && intersection->best_t != INFINITY){	//If our closest intersection is valid...
				//render light, and store the outputted colors into our pixel array
				color = render_light(object_array, object_counter, intersection->best_t, intersection->best_index, Ro, Rd, 1);
				pixel_buffer[(int)((N*M) - (floor(pixel_count/N) + 1)*N)+ pixel_count%N][0] = color[0];
				pixel_buffer[(int)((N*M) - (floor(pixel_count/N) + 1)*N)+ pixel_count%N][1] = color[1];
				pixel_buffer[(int)((N*M) - (floor(pixel_count/N) + 1)*N)+ pixel_count%N][2] = color[2];
				
				free(color);
				parse_count = 1;
			}else{
				parse_count = 1;
			}
			pixel_count++;
			free(intersection);
		}
	}
}

void create_image(double** pixel_buffer, char* output, int width, int height){	//Stores pixel array info into a .ppm file
	FILE *output_pointer = fopen(output, "wb");	/*Open the output file*/
	char buffer[width*height*3];
	int counter = 0;
	
	while(counter < width*height){	//Iterate through pixel array, and store values into a character buffer
		buffer[counter*3] = (int)(255*pixel_buffer[counter][0]);
		buffer[counter*3 + 1] = (int)(255*pixel_buffer[counter][1]);
		buffer[counter*3 + 2] = (int)(255*pixel_buffer[counter][2]);
		counter++;
	}
	fprintf(output_pointer, "P6\n%d %d\n255\n", width, height);	//Write P6 header to output.ppm
	fwrite(buffer, sizeof(char), width*height*3, output_pointer);	//Write buffer to output.ppm
	
	fclose(output_pointer);
}

void move_camera_to_front(Object** object_array, int object_count){	//Moves camera object to the front of object_array
	Object* temp_object;
	int counter = 0;
	int num_cameras = 0;
	while(counter < object_count + 1){	//Iterate through all objects in object_array
		if(object_array[counter]->kind == 0 && counter == 0){	//If first object is a camera, do nothing
			num_cameras++;
		}else if(object_array[counter]->kind == 0){		//If a camera is found further in the array, switch first object with it
			if((++num_cameras) > 1){	//But, if two cameras are ever found, throw an error
				fprintf(stderr, "Error: You may only have one camera in your .json file\n");
				exit(1);
			}
			temp_object = object_array[0];
			object_array[0] = object_array[counter];
			object_array[counter] = temp_object;
		}
		counter++;
	}
}

void main(int c, char** argv){
    Object** object_array = malloc(sizeof(Object*)*130);	//Create array of object pointers
	int width;
	int height;
	double** pixel_buffer;
	int object_counter;
	int counter = 0;
	object_array[129] = NULL;	//Indicate end of object pointer array with a NULL
	
	argument_checker(c, argv);	//Check our arguments to make sure they written correctly
	
	width = atoi(argv[1]);
	height = atoi(argv[2]);
	
	pixel_buffer = malloc(sizeof(double*)*(width*height + 1));	//Create our pixel array to hold color values
	pixel_buffer[width*height] = NULL;
	
	while(counter < width*height){	//Reserve space for each pixel in our pixel array
		pixel_buffer[counter] = malloc(sizeof(double)*3);
		pixel_buffer[counter][0] = 0;
		pixel_buffer[counter][1] = 0;
		pixel_buffer[counter][2] = 0;
		counter++;
	}
	object_counter = read_scene(argv[3], object_array);	//Parse .json scene file
	move_camera_to_front(object_array, object_counter);	//Make camera the first object in our object array
	raycast_scene(object_array, object_counter, pixel_buffer, width, height);	//Raycast our scene into the pixel array
	create_image(pixel_buffer, argv[4], width, height);	//Put info from pixel array into a P6 PPM file
    return;
}