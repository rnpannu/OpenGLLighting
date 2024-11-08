CC=g++
CFLAGS=-Iinclude -std=c++11 -g
LIBS=-lglut -lGLEW -lGL -lGLU
# Default target executed when no arguments are given to make.
default_target: as2
.PHONY : default_target

as2: as2.o InitShader.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

as2.o: as2.cc
	$(CC) $(CFLAGS) -c $^

InitShader.o: common/InitShader.cc
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f as2 *~ *.o
