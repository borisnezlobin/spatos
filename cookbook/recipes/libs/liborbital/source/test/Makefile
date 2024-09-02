# TODO

all: test_01

test_01: test_01.c
	gcc \
		-I ../ \
		-L ../target/debug/ \
		test_01.c \
		-lorbital -lc -lSDL2 -lm -ldl -lpthread \
		-o test_01
