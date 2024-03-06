all:
	gcc parse_json.c raymarch.c math_utility.c -lm -o raymarcher