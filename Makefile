CFLAGS = -lm

default: raymarcher

debug: CFLAGS += -g
debug: raymarcher

raymarcher: math_utility.a parser.o raymarch.c raymarch.h
	gcc raymarch.c $(CFLAGS) -o raymarcher math_utility.a parser.o

parser.o: Parser/parse_json.c Parser/parse_json.h
	gcc Parser/parse_json.c -c $(CFLAGS) -o parser.o

math_utility.a: Math/simple_math.c Math/simple_math.h Math/vector_math.c Math/vector_math.h Math/matrix_math.c Math/matrix_math.h
	gcc Math/simple_math.c -c $(CFLAGS) -o simple_math.o
	gcc Math/vector_math.c -c $(CFLAGS) -o vector_math.o
	gcc Math/matrix_math.c -c $(CFLAGS) -o matrix_math.o
	ar cr math_utility.a simple_math.o vector_math.o matrix_math.o
	rm simple_math.o vector_math.o matrix_math.o

*.h:
*.c:

clean:
	rm raymarcher math_utility.a parser.o