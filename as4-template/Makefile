CC=g++
CFLAGS=-Iinclude -std=c++11 -g
LIBS=-lglut -lGLEW -lGL -lGLU
# Default target executed when no arguments are given to make.
default_target: as4
.PHONY : default_target

as4: as4.o InitShader.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

as4.o: as4.cc
	$(CC) $(CFLAGS) -c $^

InitShader.o: common/InitShader.cc
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f as4 *~ *.o
