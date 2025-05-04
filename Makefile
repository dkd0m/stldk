CC = cc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Wconversion -O2 -static
objects = stldk.o

stl_btoa: stl_btoa.c stldk.o stldk.h libstldk.a
	$(CC) $(CFLAGS) stldk.o stl_btoa.c -o stl_btoa -lm

stldk.o: stldk.c stldk.h
	$(CC) $(CFLAGS) -c stldk.c -o stldk.o

libstldk.a: $(objects)
	ar rcs libstldk.a $(objects)

.PHONY: clean lib
lib: libstldk.a

clean:
	-rm $(objects) stl_btoa libstldk.a 
