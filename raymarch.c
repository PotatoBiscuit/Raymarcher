#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "math/math_utility.h"
#include "parse_json.h"
#include "raymarch.h"

//These variables should NOT be changed after parsing the json file
Object* object_array[130];
int object_counter;

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

double apply_transformations(double* position, double* object_position, double* object_rotation ){
	position[0] -= object_position[0];
	position[1] -= object_position[1];
	position[2] -= object_position[2];
	apply_xyz_rotation( position, object_rotation );
}

double sphere_sdf(double* position, double radius){ //Calculate how far our ray position is from the sphere
	return magnitude(position) - radius;
}

double plane_sdf( double* position, double* plane_normal ){
	return dot_product( position, plane_normal );
}

double box_sdf( double* position, double* dimensions ){
	double distance_vect[3];
	distance_vect[0] = fabs( position[0] ) - dimensions[0];
	distance_vect[1] = fabs( position[1] ) - dimensions[1];
	distance_vect[2] = fabs( position[2] ) - dimensions[2];

	double pos_distance_vect[3];
	pos_distance_vect[0] = max( distance_vect[0], 0.0 );
	pos_distance_vect[1] = max( distance_vect[1], 0.0 );
	pos_distance_vect[2] = max( distance_vect[2], 0.0 );
	double temp_distance = magnitude( pos_distance_vect );
	return temp_distance + min( max( distance_vect[0], max( distance_vect[0], max( distance_vect[1], distance_vect[2] ) ) ), 0.0 );
}

double donut_sdf( double* position, double radius, double thickness ){
	double xz[2] = {position[0], position[2]};
	double diameter = radius * 2;
	double donut_bounds[2] = { magnitude_2D(xz) - diameter, position[1] };
	return magnitude_2D( donut_bounds ) - thickness;
}

double mandelbulb_sdf( double* position ){
	double temp_pos[3] = {position[0], position[1], position[2]};

	double dr = 1.0;
	double r;
	double power = 8.0;
	for( int i = 0; i < 20; i++ ) {
		r = sqrt( dot_product(temp_pos, temp_pos) );
		if( r > 2.0 ){ break; }

		double theta = acos( temp_pos[2] / r ) * power;
		double phi = atan2(temp_pos[1], temp_pos[0]) * power;
		dr = pow( r, power - 1.0 ) * power * dr + 1.0;

		double zr = pow( r, power );

		temp_pos[0] = position[0] + zr * sin(theta) * cos(phi);
		temp_pos[1] = position[1] + zr * sin(theta) * sin(phi);
		temp_pos[2] = position[2] + zr * cos(theta);
	}
	return 0.5 * log(r)*r/dr;
}

double infinite_shape( double* position, double tile_size ){
	int unit_pos[3] = { 
		(int) round(position[0] / tile_size),
		(int) round(position[1] / tile_size),
		(int) round(position[2] / tile_size)
	};
	position[0] -= tile_size * unit_pos[0];
	position[1] -= tile_size * unit_pos[1];
	position[2] -= tile_size * unit_pos[2];
}

void store_obj_data( double temp_distance, double temp_min_distance, int obj_index, Intersect* intersect ){
	if( intersect != NULL ){
		if( temp_distance < temp_min_distance ){
			intersect->best_index = obj_index;
			intersect->min_distance = temp_distance;
		}
	}
}

// There are TWO ways to get results from this function, the double using normal return logic, and the Intersect* arg for extra object data
double all_intersections( double* position, Intersect* intersect ){
    double temp_distance;
	double temp_min_distance = INFINITY;
	int parse_count = 1;
	while( parse_count < object_counter + 1 ){	//do the raymarching with a ray
		double temp_position[3] = {position[0], position[1], position[2]};
		if( object_array[parse_count]->infinite_interval > 0 ){
			infinite_shape( temp_position, object_array[parse_count]->infinite_interval );
		}
		apply_transformations( temp_position, object_array[parse_count]->position, object_array[parse_count]->rotation );

		if( object_array[parse_count]->kind == Sphere ){
			temp_distance = sphere_sdf( temp_position, object_array[parse_count]->sphere.radius );

			store_obj_data( temp_distance, temp_min_distance, parse_count, intersect );
			temp_min_distance = min( temp_distance, temp_min_distance );

		}else if( object_array[parse_count]->kind == Plane ){ //See if a plane overshadows our point of intersection
			temp_distance = plane_sdf( temp_position, object_array[parse_count]->plane.normal );

			store_obj_data( temp_distance, temp_min_distance, parse_count, intersect );
			temp_min_distance = min( temp_distance, temp_min_distance );

		}else if( object_array[parse_count]->kind == Donut ){
			temp_distance = donut_sdf( temp_position, object_array[parse_count]->donut.radius,
										object_array[parse_count]->donut.thickness );

			store_obj_data( temp_distance, temp_min_distance, parse_count, intersect );
			temp_min_distance = min( temp_distance, temp_min_distance );

		}else if( object_array[parse_count]->kind == Box ){
			temp_distance = box_sdf( temp_position, object_array[parse_count]->box.dimensions );
			
			store_obj_data( temp_distance, temp_min_distance, parse_count, intersect );
			temp_min_distance = min( temp_distance, temp_min_distance );

		}else if( object_array[parse_count]->kind == Mandelbulb ){
			temp_distance = mandelbulb_sdf( temp_position );
			
			store_obj_data( temp_distance, temp_min_distance, parse_count, intersect );
			temp_min_distance = min( temp_distance, temp_min_distance );

		}else{	//If a light was found, skip it
			//do nothing
		}
		parse_count++;
	}
	return temp_min_distance;
}

Intersect* raymarch(double* Ro, double* Rd){	//Find object intersections
	Intersect* intersection = malloc(sizeof(Intersect));
	int num_steps = 0;

	intersection->min_distance = INFINITY;
	intersection->position[0] = Ro[0];
	intersection->position[1] = Ro[1];
	intersection->position[2] = Ro[2];
	
    while(num_steps++ < MAX_STEPS){
		all_intersections( intersection->position, intersection );
        intersection->position[0] += Rd[0]*intersection->min_distance;
        intersection->position[1] += Rd[1]*intersection->min_distance;
        intersection->position[2] += Rd[2]*intersection->min_distance;
        if( intersection->min_distance < INTERSECTION_LIMIT || intersection->min_distance > OUTER_BOUNDS ) {
            break;
        }
    }

	return intersection;
}

void intersect_normal( double* normal, double* intersect_pos ){
	double sampling_interval = .0001;

	//Intersect coordinates to make things easier to read
	double x = intersect_pos[0];
	double y = intersect_pos[1];
	double z = intersect_pos[2];

	normal[0] = all_intersections((double[3]){x + sampling_interval, y, z}, NULL) -
				all_intersections((double[3]){x - sampling_interval, y, z}, NULL);
	normal[1] = all_intersections((double[3]){x, y + sampling_interval, z}, NULL) -
				all_intersections((double[3]){x, y - sampling_interval, z}, NULL);
	normal[2] = all_intersections((double[3]){x, y, z + sampling_interval}, NULL) -
				all_intersections((double[3]){x, y, z - sampling_interval}, NULL);

	normalize(normal);
}

Object* find_light(){ // Just find 1 light for now
	int parse_count = 1;
	while(parse_count++ < object_counter + 1){
		if(object_array[parse_count]->kind == Light) {
			return object_array[parse_count];
		}
	}
}

double calculate_shadow( double* light_pos, double* light_direction, double* intersect_pos ){
	Intersect* light_collision = raymarch(light_pos, light_direction);
	if( distance_between( light_collision->position, intersect_pos ) <= 1 ){
		free(light_collision);
		return 1.0;
	}
	free(light_collision);
	return 0.25; //Reduce color to 25% brightness
}

void diffuse_color( double* color, double* normal, double* intersect_to_light, Object* light, Object* object ){
	double diffuse_intensity = clamp( dot_product( normal, intersect_to_light ) );

	color[0] = light->light.color[0] * object->diffuse_color[0] * diffuse_intensity;
	color[1] = light->light.color[1] * object->diffuse_color[1] * diffuse_intensity;
	color[2] = light->light.color[2] * object->diffuse_color[2] * diffuse_intensity;
}

void specular_color( double* color, double* camera_direction, double* normal, double* intersect_to_light, Object* light, Object* object ){
	double reflected_vector[3];
	reflect( intersect_to_light, normal, reflected_vector );

	double specular_intensity = pow( max( 0,dot_product( reflected_vector, camera_direction ) ), object->shininess );

	color[0] += specular_intensity * light->light.color[0] * min(1, object->shininess / 10.0);
	color[1] += specular_intensity * light->light.color[1] * min(1, object->shininess / 10.0);
	color[2] += specular_intensity * light->light.color[2] * min(1, object->shininess / 10.0);
}

void calculate_color( double* camera_direction, double* color, Intersect* intersection ){
	double normal[3] = {0.0, 0.0, 0.0};
	intersect_normal(normal, intersection->position);

	Object* light = find_light();

	double intersect_to_light[3];
	intersect_to_light[0] = light->position[0] - intersection->position[0];
	intersect_to_light[1] = light->position[1] - intersection->position[1];
	intersect_to_light[2] = light->position[2] - intersection->position[2];
	normalize( intersect_to_light );

	double light_to_intersect[3] = {-intersect_to_light[0], -intersect_to_light[1], -intersect_to_light[2]};

	diffuse_color( color, normal, intersect_to_light, light, object_array[ intersection->best_index ] );
	specular_color( color, camera_direction, normal, intersect_to_light, light, object_array[ intersection->best_index ] );

	double shadow_mult = calculate_shadow( light->position, light_to_intersect, intersection->position );
	vector_mult( color, shadow_mult );

	color[0] = clamp( color[0] );
	color[1] = clamp( color[1] );
	color[2] = clamp( color[2] );
}

void raymarch_scene(double** pixel_buffer, int N, int M){	//This raymarches our object_array
	int parse_count = 0;
	int pixel_count = 0;
	int i;
	double Ro[3];
	double Rd[3];
	double color[3] = {0,0,0};
	double cx = 0;
	double cy = 0;
	double w;
	double h;
	double pixwidth;
	double pixheight;
	Intersect* intersection;
	
	if(object_array[parse_count]->kind != Camera){	//If camera is not present, throw an error
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
	
	for(int y = 0; y < M; y += 1){	//Raymarch for every pixel
		for(int x = 0; x < N; x += 1){
			Rd[0] = cx - (w/2) + pixwidth * (x + .5);	//Create direction vector
			Rd[1] = cy - (h/2) + pixheight * (y + .5);
			Rd[2] = 1;
			normalize(Rd);
			intersection = raymarch(Ro, Rd);

			
			if(intersection->min_distance <= OUTER_BOUNDS){	//If our closest intersection is valid...
				calculate_color(Rd, color, intersection);

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

void move_camera_to_front(){	//Moves camera object to the front of object_array
	Object* temp_object;
	int counter = 0;
	int num_cameras = 0;
	while(counter < object_counter + 1){	//Iterate through all objects in object_array
		if(object_array[counter]->kind == Camera && counter == 0){	//If first object is a camera, do nothing
			num_cameras++;
		}else if(object_array[counter]->kind == Camera){		//If a camera is found further in the array, switch first object with it
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
	int width;
	int height;
	double** pixel_buffer;
	int counter = 0;
	
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
	move_camera_to_front();	//Make camera the first object in our object array
	raymarch_scene(pixel_buffer, width, height);	//Raycast our scene into the pixel array
	create_image(pixel_buffer, argv[4], width, height);	//Put info from pixel array into a P6 PPM file

	// I would free pixel_buffer here, but we're not creating any more of it, and the main function is ending anyways
    return;
}