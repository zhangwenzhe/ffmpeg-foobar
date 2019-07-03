object = lib demo

lib:
	g++ -fPIC -shared -o libmymatcher.so `pkg-config --cflags opencv` mymatcher.hpp mymatcher.cpp mymatcher.c mymatcher.h `pkg-config --libs opencv`
demo:
	g++ `pkg-config --cflags opencv` main.c `pkg-config --libs opencv` -L. -lmymatcher -o demo

all: $(object)

#g++  main.cpp  -L. -lmymatcher -o demo
#g++ `pkg-config --cflags opencv4` demo.cpp `pkg-config --libs opencv4` -o demo
