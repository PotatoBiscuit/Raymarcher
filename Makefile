all:
	gcc parse_json.c parse_json.h raymarch.c math_utility.h math_utility.c -lm -o raymarcher