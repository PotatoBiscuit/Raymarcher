#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "math_utility.h"
#include "parse_json.h"

int line = 1;	//Line currently being parsed

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
    int c = fgetc(json);
#ifdef DEBUG
    printf("next_c: '%c'\n", c);
#endif
    if (c == '\n') {
        line += 1;
    }
    if (c == EOF) {
        fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
        exit(1);
    }
    return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
    int c = next_c(json);
    if (c == d) return;
    fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
    exit(1);    
}

// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
    char buffer[129];
    int c = next_c(json);
    if (c != '"') {
        fprintf(stderr, "Error: Expected string on line %d.\n", line);
        exit(1);
    }  
    c = next_c(json);
    int i = 0;
    while (c != '"') {
        if (i >= 128) {	//Strings must be shorter than 128 characters
        fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
        exit(1);      
        }
        if (c == '\\') {	//No escape characters allowed
        fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
        exit(1);      
        }
        if (c < 32 || c > 126) {	//String characters must be ascii
        fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
        exit(1);
        }
        buffer[i] = c;
        i += 1;
        c = next_c(json);
    }
    buffer[i] = 0;
    return strdup(buffer);	//Return the string in a duplicated buffer
}

double next_number(FILE* json) {	//Parse the next number and return it as a double
	double value;
	int numDigits = 0;
	numDigits = fscanf(json, "%lf", &value);
	if(numDigits == 0){
		fprintf(stderr, "Error: Expected number at line %d\n", line);
		exit(1);
	}
	return value;
}

// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
    int c = next_c(json);
    while (isspace(c)) {
        c = next_c(json);
    }
    ungetc(c, json);
}


double* next_vector(FILE* json) {	//parse the next vector, and return it in double form
    double* v = malloc(3*sizeof(double));
    expect_c(json, '[');
    skip_ws(json);
    v[0] = next_number(json);
    skip_ws(json);
    expect_c(json, ',');
    skip_ws(json);
    v[1] = next_number(json);
    skip_ws(json);
    expect_c(json, ',');
    skip_ws(json);
    v[2] = next_number(json);
    skip_ws(json);
    expect_c(json, ']');
    return v;
}

void store_common_fields(Object* input_object, int type_of_field, double input_value, double* input_vector){
    if(type_of_field == Diffuse_Color){
        if(input_vector[0] > 1 || input_vector[1] > 1 || input_vector[2] > 1){
            fprintf(stderr, "Error: Diffuse color values must be between 0 and 1, line:%d\n", line);
            exit(1);
        }
        if(input_vector[0] < 0 || input_vector[1] < 0 || input_vector[2] < 0){
            fprintf(stderr, "Error: Diffuse color values may not be negative, line:%d\n", line);
            exit(1);
        }
        input_object->diffuse_color[0] = input_vector[0];
        input_object->diffuse_color[1] = input_vector[1];
        input_object->diffuse_color[2] = input_vector[2];
    }else if(type_of_field == Specular_Color){
        if(input_vector[0] > 1 || input_vector[1] > 1 || input_vector[2] > 1){
            fprintf(stderr, "Error: Specular color values must be between 0 and 1, line:%d\n", line);
            exit(1);
        }
        if(input_vector[0] < 0 || input_vector[1] < 0 || input_vector[2] < 0){
            fprintf(stderr, "Error: Specular color values may not be negative, line:%d\n", line);
            exit(1);
        }
        input_object->specular_color[0] = input_vector[0];
        input_object->specular_color[1] = input_vector[1];
        input_object->specular_color[2] = input_vector[2];
    }else if(type_of_field == Position){
        input_object->position[0] = input_vector[0];
        input_object->position[1] = input_vector[1];
        input_object->position[2] = input_vector[2];
    }else if(type_of_field == Shininess){
        input_object->shininess = input_value;
    }else if(type_of_field == Ior){
        if(input_value < 1) input_value = 1;
        input_object->ior = input_value;
    }else if(type_of_field == Infinite_Interval){
        if(input_value > 0) input_object->infinite_interval = input_value;
    }
}

//This function takes an input value or vector, and puts it into our object array
void store_value(Object* input_object, int type_of_field, double input_value, double* input_vector){
	//if input_value or input_vector aren't used, a 0 or NULL value should be passed in
	if( input_object->kind == Camera ){	//If the object is a camera, store the input into its width or height fields
		if(type_of_field == Width){
			if(input_value <= 0){
				fprintf(stderr, "Error: Camera width must be greater than 0, line:%d\n", line);
				exit(1);
			}
			input_object->camera.width = input_value;
		}else if(type_of_field == Height){
			if(input_value <= 0){
				fprintf(stderr, "Error: Camera height must be greater than 0, line:%d\n", line);
				exit(1);
			}
			input_object->camera.height = input_value;
		}else{
			fprintf(stderr, "Error: Camera may only have 'width' or 'height' fields, line:%d\n", line);
			exit(1);
		}
	}else if( input_object->kind == Sphere ){	//If the object is a sphere, store input into its respective fields
        store_common_fields(input_object, type_of_field, input_value, input_vector);
		if(type_of_field == Radius){
			input_object->sphere.radius = input_value;
		}
	}else if( input_object->kind == Plane ){	//If the object is a plane, store input into its respective fields
		store_common_fields(input_object, type_of_field, input_value, input_vector);
		if(type_of_field == Normal){
			if(input_vector[2] > 0){
				input_object->plane.normal[0] = -input_vector[0];
				input_object->plane.normal[1] = -input_vector[1];
				input_object->plane.normal[2] = -input_vector[2];
			}else{
				input_object->plane.normal[0] = input_vector[0];
				input_object->plane.normal[1] = input_vector[1];
				input_object->plane.normal[2] = input_vector[2];
			}
			normalize(input_object->plane.normal);
		}
    }else if( input_object->kind == Donut ){
        store_common_fields(input_object, type_of_field, input_value, input_vector);
        if( type_of_field == Radius ){
            input_object->donut.radius = input_value;
        }else if( type_of_field == Thickness ){
            input_object->donut.thickness = input_value;
        }
    }else if ( input_object->kind == Box ){
        store_common_fields(input_object, type_of_field, input_value, input_vector);
        if( type_of_field == Dimensions ){
            input_object->box.dimensions[0] = input_vector[0];
            input_object->box.dimensions[1] = input_vector[1];
            input_object->box.dimensions[2] = input_vector[2];
            if( input_vector[0] <= 0.0 && input_vector[1] <= 0.0 && input_vector[2] <= 0.0 ){
                fprintf( stderr, "Error: One of your box dimensions was set to zero, there is a decent chance it fails to render\n" );
            }

        }
    }else if ( input_object->kind == Mandelbulb ){
        store_common_fields(input_object, type_of_field, input_value, input_vector);
        if( type_of_field  == Rotation ){
            input_object->mandelbulb.rotation[0] = input_vector[0];
            input_object->mandelbulb.rotation[1] = input_vector[1];
            input_object->mandelbulb.rotation[2] = input_vector[2];
            vect_degrees_to_radians( input_object->mandelbulb.rotation );
        }
	}else if(input_object->kind == Light){	//If object is a light, store input into its respective fields
        store_common_fields(input_object, type_of_field, input_value, input_vector);
		if(type_of_field == Color){
			input_object->light.color[0] = input_vector[0];
			input_object->light.color[1] = input_vector[1];
			input_object->light.color[2] = input_vector[2];
		}else if(type_of_field == Direction){
			input_object->light.direction[0] = input_vector[0];
			input_object->light.direction[1] = input_vector[1];
			input_object->light.direction[2] = input_vector[2];
			normalize(input_object->light.direction);
		}else if(type_of_field == Radial_A0){
			input_object->light.radial_a0 = input_value;
		}else if(type_of_field == Radial_A1){
			input_object->light.radial_a1 = input_value;
		}else if(type_of_field == Radial_A2){
			input_object->light.radial_a2 = input_value;
		}else if(type_of_field == Angular_A0){
			input_object->light.angular_a0 = input_value;
		}else if(type_of_field == Theta){
			input_object->light.theta = input_value;
		}
	}else{
		fprintf(stderr, "Error: Undefined object type, line:%d\n", line);
		exit(1);
	}
}

int read_scene(char* filename, Object** object_array) {	//Parses json file, and stores object information into object_array
    int c;
    int num_objects = 0;
    int object_counter = -1;
    int height = 0, width = 0, radius = 0, diffuse_color = 0, specular_color = 0, position = 0, normal = 0;	//These will serve as boolean operators
    int radial_a2 = 0, radial_a1 = 0, radial_a0 = 0, angular_a0 = 0, color = 0, theta = 0, ior = 0;
    FILE* json = fopen(filename, "r");	//Open our json file

    if (json == NULL) {	//If the file does not exist, throw an error
        fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
        exit(1);
    }
    
    skip_ws(json);
    
    // Find the beginning of the list
    expect_c(json, '[');

    skip_ws(json);

    // Find the objects
    while (1) {
        c = fgetc(json);
        if (c == ']' && num_objects != 0) {		//A ',' must be read before getting here, which means we are expecting more objects
            fprintf(stderr, "Error: End of file reached when expecting more objects, line:%d\n", line);
            exit(1);
        }
        else if(c == ']'){	//If no objects have been parsed and a bracket is found, our file is empty, throw an error
            fprintf(stderr, "Error: JSON file contains no objects\n");
            fclose(json);
            exit(1);
        }
        
        if (c == '{') {	//Start object parsing
        if(object_counter >= 128){	//If 128 objects have already been scanned, throw an error
            fprintf(stderr, "Error: Maximum amount of objects allowed (not including the camera) is 128, line:%d\n", line);
            exit(1);
        }
        object_array[++object_counter] = malloc(sizeof(Object)); //Make space for the new object in object_array
        skip_ws(json);
        
        // Parse object type
        char* key = next_string(json);
        if (strcmp(key, "type") != 0) {
            fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
            exit(1);
        }

        skip_ws(json);

        expect_c(json, ':');

        skip_ws(json);

        char* value = next_string(json);

        if (strcmp(value, "camera") == 0) {
            object_array[object_counter]->kind = Camera;
            width = 1;
            height = 1;
        } else if (strcmp(value, "sphere") == 0) {
            object_array[object_counter]->kind = Sphere;
            position = 1;
            radius = 1;
            specular_color = 1;
            diffuse_color = 1;
            ior = 1;
        } else if (strcmp(value, "plane") == 0) {
            object_array[object_counter]->kind = Plane;
            position = 1;
            normal = 1;
            specular_color = 1;
            diffuse_color = 1;
            ior = 1;
        } else if (strcmp(value, "donut") == 0) {
            object_array[object_counter]->kind = Donut;
            position = 1;
            specular_color = 1;
            diffuse_color = 1;
            ior = 1;
        } else if (strcmp(value, "box") == 0) {
            object_array[object_counter]->kind = Box;
            position = 1;
            specular_color = 1;
            diffuse_color = 1;
            ior = 1;
        } else if (strcmp(value, "mandelbulb") == 0) {
            object_array[object_counter]->kind = Mandelbulb;
            position = 1;
            specular_color = 1;
            diffuse_color = 1;
            ior = 1;
        } else if (strcmp(value, "light") == 0){
            object_array[object_counter]->kind = Light;
            position = 1;
            color = 1;
            radial_a0 = 1;
            radial_a1 = 1;
            radial_a2 = 1;
            angular_a0 = 1;
            theta = 1;
        } else {
            fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
            exit(1);
        }

        skip_ws(json);

        while (1) {	//Parse the rest of the object
            
            c = next_c(json);
            if (c == '}') {
            // stop parsing this object
            //If a required field is missing from an object, throw an error
            if(height == 1 || width == 1 || position == 1 || normal == 1 || color == 1 || radius == 1 ||
            diffuse_color == 1 || specular_color == 1 || position == 1){	//If a required value was not in the json file, throw error
                fprintf(stderr, "Error: Required field missing from object at line:%d\n", line);
                exit(1);
            }
            if(radial_a0 == 1){	//If radial_a0 did not exist in json file, store the default value 1
                store_value(object_array[object_counter], Radial_A0, 1, NULL);
                radial_a0 = 0;
            }
            if(radial_a1 == 1){	//If radial_a1 did not exist in json file, store the default value 0
                store_value(object_array[object_counter], Radial_A1, 0, NULL);
                radial_a1 = 0;
            }
            if(radial_a2 == 1){	//If radial_a2 did not exist in json file, store the default value 0
                store_value(object_array[object_counter], Radial_A2, 0, NULL);
                radial_a2 = 0;
            }
            if(angular_a0 == 1){	//If angular_a0 did not exist in json file, store default value 0
                store_value(object_array[object_counter], Angular_A0, 0, NULL);
                angular_a0 = 0;
            }
            if(theta == 1){	//If theta did not exist in json file, store default value 0
                store_value(object_array[object_counter], Theta, 0, NULL);
                theta = 0;
            }
            if(ior == 1){
                store_value(object_array[object_counter], Ior, 1, NULL);
                ior = 0;
            }
            break;
            } else if (c == ',') {
                // read another field
                skip_ws(json);
                char* key = next_string(json);
                skip_ws(json);
                expect_c(json, ':');
                skip_ws(json);
                if (strcmp(key, "width") == 0){	//Based on the field, parse a number or vector
                    store_value(object_array[object_counter], Width, next_number(json), NULL);	//And store the value in the object_array
                    width = 0;
                }else if(strcmp(key, "height") == 0){
                    store_value(object_array[object_counter], Height, next_number(json), NULL);
                    height = 0;
                }else if(strcmp(key, "radius") == 0) {
                    store_value(object_array[object_counter], Radius, next_number(json), NULL);
                    radius = 0;
                }else if (strcmp(key, "color") == 0){
                    store_value(object_array[object_counter], Color, 0, next_vector(json));
                    color = 0;
                }else if(strcmp(key, "position") == 0){
                    store_value(object_array[object_counter], Position, 0, next_vector(json));
                    position = 0;
                }else if(strcmp(key, "normal") == 0) {
                    store_value(object_array[object_counter], Normal, 0, next_vector(json));
                    normal = 0;
                }else if(strcmp(key, "diffuse_color") == 0){
                    store_value(object_array[object_counter], Diffuse_Color, 0, next_vector(json));
                    diffuse_color = 0;
                }else if(strcmp(key, "specular_color") == 0){
                    store_value(object_array[object_counter], Specular_Color, 0, next_vector(json));
                    specular_color = 0;
                }else if(strcmp(key, "radial-a0") == 0){
                    store_value(object_array[object_counter], Radial_A0, next_number(json), NULL);
                    radial_a0 = 0;
                }else if(strcmp(key, "radial-a1") == 0){
                    store_value(object_array[object_counter], Radial_A1, next_number(json), NULL);
                    radial_a1 = 0;
                }else if(strcmp(key, "radial-a2") == 0){
                    store_value(object_array[object_counter], Radial_A2, next_number(json), NULL);
                    radial_a2 = 0;
                }else if(strcmp(key, "angular-a0") == 0){
                    store_value(object_array[object_counter], Angular_A0, next_number(json), NULL);
                    angular_a0 = 0;
                }else if(strcmp(key, "direction") == 0){
                    store_value(object_array[object_counter], Direction, 0, next_vector(json));
                }else if(strcmp(key, "rotation") == 0){
                    store_value(object_array[object_counter], Rotation, 0, next_vector(json));
                }else if(strcmp(key, "dimensions") == 0){
                    store_value(object_array[object_counter], Dimensions, 0, next_vector(json));
                }else if(strcmp(key, "theta") == 0){
                    double value = next_number(json);
                    store_value(object_array[object_counter], Theta, degrees_to_radians(value), NULL);
                    theta = 0;
                }else if(strcmp(key, "shininess") == 0){
                    store_value(object_array[object_counter], Shininess, next_number(json), NULL);
                }else if(strcmp(key, "thickness") == 0){
                    store_value(object_array[object_counter], Thickness, next_number(json), NULL);
                }else if(strcmp(key, "ior") == 0){
                    store_value(object_array[object_counter], Ior, next_number(json), NULL);
                    ior = 0;
                }else if(strcmp(key, "infinite_interval") == 0){
                    store_value( object_array[object_counter], Infinite_Interval, next_number(json), NULL);
                }else{	//If there was an invalid field, throw an error
                        fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
                        key, line);
                        exit(1);
                }
                skip_ws(json);
            } else {	//If a ',' or '}' was not received, throw an error
                fprintf(stderr, "Error: Unexpected value on line %d\n", line);
                exit(1);
            }
        }
        skip_ws(json);
        num_objects++;
        c = next_c(json);
        if (c == ',') {	//If there is a comma, parse more objects!

        skip_ws(json);
        } else if (c == ']') {	//If there is an ending bracket, it is the end JSON file
        fclose(json);
        return object_counter;
        } else {	//Throw error if we don't encounter a ',' or ']'
            fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
            exit(1);
        }
        }
    }
    line = 1;
}