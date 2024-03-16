CFLAGS = -lm

default: raymarcher

debug: CFLAGS += -g
debug: raymarcher

raymarcher: math_utility.o parser.o raymarch.c
	gcc raymarch.c $(CFLAGS) -o raymarcher math_utility.o parser.o

parser.o: Parser/parse_json.c
	gcc Parser/parse_json.c -c $(CFLAGS) -o parser.o

math_utility.o: Math/math_utility.c
	gcc Math/math_utility.c -c $(CFLAGS) -o math_utility.o

Math/math_utility.c:
Parser/parse_json.c:
raymarch.c:

clean:
	rm raymarcher math_utility.o parser.o