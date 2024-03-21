CFLAGS = -lm
BUILD = ./build

default: ${BUILD} raymarcher

debug: CFLAGS += -g
debug: default

raymarcher: ${BUILD}/math_utility.a ${BUILD}/parser.o raymarch.c raymarch.h
	gcc raymarch.c $(CFLAGS) -o raymarcher ${BUILD}/math_utility.a ${BUILD}/parser.o

${BUILD}/parser.o: Parser/parse_json.c Parser/parse_json.h
	gcc Parser/parse_json.c -c $(CFLAGS) -o ${BUILD}/parser.o

${BUILD}/math_utility.a: Math/simple_math.c Math/simple_math.h Math/vector_math.c Math/vector_math.h Math/matrix_math.c Math/matrix_math.h
	gcc Math/simple_math.c -c $(CFLAGS) -o ${BUILD}/simple_math.o
	gcc Math/vector_math.c -c $(CFLAGS) -o ${BUILD}/vector_math.o
	gcc Math/matrix_math.c -c $(CFLAGS) -o ${BUILD}/matrix_math.o
	ar cr ${BUILD}/math_utility.a ${BUILD}/simple_math.o ${BUILD}/vector_math.o ${BUILD}/matrix_math.o

${BUILD}:
	mkdir ${BUILD}

*.h:
*.c:

clean:
	rm raymarcher
	rm -r ${BUILD}