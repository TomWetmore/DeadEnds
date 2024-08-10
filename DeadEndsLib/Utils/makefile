CC=clang
CFLAGS=-c -g -Wall
INCLUDES=-I./Includes -I../DataTypes/Includes
AR=ar
ARFLAGS=-cr
OFILES=errors.o standard.o unicode.o path.o utils.o file.o
LIBNAME=utils

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<

clean:
	rm *.o *.a
