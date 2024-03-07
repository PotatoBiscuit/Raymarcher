default:
	gcc parse_json.c raymarch.c math_utility.c -lm -o raymarcher
debug:
	gcc parse_json.c raymarch.c math_utility.c -g -lm -o raymarcher