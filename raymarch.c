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
#define INTERSECTION_LIMIT .0001
#define OUTER_BOUNDS 1000000

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

double sphere_intersection(double* position, double* sphere_position, double radius){ //Calculate how far our ray position is from the sphere
    double distance = distance_between(sphere_position, position);
    return distance - radius;
}

Tuple* shoot(Object** object_array, int object_counter, double* Ro, double* Rd){	//Find object intersections
	Tuple* intersection = malloc(sizeof(Tuple));
	int parse_count = 1;
	double closest_distance = INFINITY;
	int best_index = -1;

    double temp_ray_position[3] = {Ro[0], Ro[1], Ro[2]};
	
    while(1){
        double temp_distance = INFINITY;
        double temp_min_distance = INFINITY;
        while(parse_count < object_counter + 1){	//do the raymarching with a ray

            if(object_array[parse_count]->kind == 1){	//See if a sphere overshadows our point of intersection
                temp_distance = sphere_intersection(temp_ray_position, object_array[parse_count]->sphere.position,
                            object_array[parse_count]->sphere.radius);
                if( temp_distance > 0 ) {
                    temp_min_distance = min( temp_distance, temp_min_distance );
                    best_index = parse_count;
                }

            }else if(object_array[parse_count]->kind == 2){ //See if a plane overshadows our point of intersection
                //plane intersection
            }else{	//If a light was found, skip it
                //do nothing
            }

            parse_count++;
	    }

        temp_ray_position[0] += Rd[0]*temp_min_distance;
        temp_ray_position[1] += Rd[1]*temp_min_distance;
        temp_ray_position[2] += Rd[2]*temp_min_distance;
        if( temp_min_distance < INTERSECTION_LIMIT || temp_min_distance > OUTER_BOUNDS ) {
            if( temp_min_distance > OUTER_BOUNDS ) {
                closest_distance = -1;
            }
            else if( object_array[best_index]->kind == 1 ){
                closest_distance = sphere_intersection(Ro, object_array[best_index]->sphere.position,
                    object_array[best_index]->sphere.radius);
            }
            break;
        }
        parse_count = 1;
    }

	intersection->best_index = best_index;
	intersection->best_t = closest_distance;
	return intersection;
}

void raymarch_scene(Object** object_array, int object_counter, double** pixel_buffer, int N, int M){	//This raymarches our object_array
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
				//render color here
                color[0] = 0;
                color[1] = 0;
                color[2] = 0;
                if( intersection->best_t > 0 ){
                    color[0] = 155;
                    color[1] = 155;
                    color[2] = 155;
                }

				pixel_buffer[(int)((N*M) - (floor(pixel_count/N) + 1)*N)+ pixel_count%N][0] = color[0];
				pixel_buffer[(int)((N*M) - (floor(pixel_count/N) + 1)*N)+ pixel_count%N][1] = color[1];
				pixel_buffer[(int)((N*M) - (floor(pixel_count/N) + 1)*N)+ pixel_count%N][2] = color[2];
				
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
	raymarch_scene(object_array, object_counter, pixel_buffer, width, height);	//Raycast our scene into the pixel array
	create_image(pixel_buffer, argv[4], width, height);	//Put info from pixel array into a P6 PPM file
    return;
}