APPNAME=Mandelbrot
CC=g++
CFLAGS=-Wall -std=c++11

ODIR=obj

LIBS=-lsfml-graphics -lsfml-window -lsfml-system -pthread -lX11

all: $(APPNAME)

$(APPNAME):
	$(CC) -o $(APPNAME) $(CFLAGS) src/*.cpp $(LIBS)

.PHONY: clean

clean:
	rm -f Mandelbrot