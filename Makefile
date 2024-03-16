CFLAGS = -lm

default: raymarcher

debug: CFLAGS += -g
debug: raymarcher

raymarcher: math_utility.o
	gcc parse_json.c raymarch.c $(CFLAGS) -o raymarcher math_utility.o

math_utility.o: math_utility.c
	gcc math/math_utility.c -c $(CFLAGS) -o math_utility.o

math_utility.c:
	

clean:
	rm raymarcher math_utility.o