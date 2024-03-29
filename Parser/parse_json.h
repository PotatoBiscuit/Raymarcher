#ifndef PARSE_JSON
#define PARSE_JSON

typedef enum {
	Camera,
	Sphere,
	Plane,
	Box,
	Donut,
	Cone,
	EternalCylinder,
	Mandelbulb,
	Light
} Primitive;

typedef struct {	//Create structure to be used for our object_array
	Primitive kind; // 0 = camera, 1 = sphere, 2 = plane, 3 = light
	double diffuse_color[3];
	double specular_color[3];
	double position[3];
	double rotation[3]; // In degrees not radians
	double shininess;
	double ior;
	double infinite_interval;
	union {
		struct {
			double width;
			double height;
		} camera;
		struct {
			double radius;
		} sphere;
		struct {
			double normal[3];
		} plane;
		struct {
			double radius;
			double thickness;
		} donut;
		struct {
			double dimensions[3];
		} box;
		struct {
			double angle; // In degrees not radians
			double height;
		} cone;
		struct {
			double radius;
		} eternal_cylinder;
		struct {
			// Put any mandelbulb specific fields here
		} mandelbulb;
		struct {
			double color[3];
			double direction[3];
			double radial_a2;
			double radial_a1;
			double radial_a0;
			double angular_a0;
			double theta;
		} light;
	};
} Object;

int read_scene(char* filename, Object** object_array);

typedef enum {
	Width,
	Height,
	Radius,
	Diffuse_Color,
	Specular_Color,
	Position,
	Normal,
	Radial_A0,
	Radial_A1,
	Radial_A2,
	Angular_A0,
	Color,
	Direction,
	Rotation,
	Dimensions,
	Theta,
	Shininess,
	Thickness,
	Angle,
	Ior,
	Infinite_Interval
} FieldType;

#endif