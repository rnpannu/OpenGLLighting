CC=g++
CFLAGS=-Iinclude -std=c++11 -g
LIBS=-lglut -lGLEW -lGL -lGLU
# Default target executed when no arguments are given to make.
default_target: trafficlight
.PHONY : default_target

trafficlight: trafficlight.o light.o camera.o InitShader.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

trafficlight.o: trafficlight.cc 
	$(CC) $(CFLAGS) -c trafficlight.cc

light.o: light.cc 
	$(CC) $(CFLAGS) -c light.cc

camera.o: camera.cc 
	$(CC) $(CFLAGS) -c camera.cc

trafficlight.o: light.h camera.h
light.o: light.h
camera.o: camera.h

InitShader.o: common/InitShader.cc
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f trafficlight *~ *.o
