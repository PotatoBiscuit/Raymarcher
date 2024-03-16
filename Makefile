CFLAGS = -lm

default: raymarcher

debug: CFLAGS += -g
debug: raymarcher

raymarcher: math_utility.o parser.o raymarch.c raymarch.h
	gcc raymarch.c $(CFLAGS) -o raymarcher math_utility.o parser.o

parser.o: Parser/parse_json.c Parser/parse_json.h
	gcc Parser/parse_json.c -c $(CFLAGS) -o parser.o

math_utility.o: Math/math_utility.c Math/math_utility.h
	gcc Math/math_utility.c -c $(CFLAGS) -o math_utility.o

*.h:
*.c:

clean:
	rm raymarcher math_utility.o parser.o