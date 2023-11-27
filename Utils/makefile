CC=clang
CFLAGS=-c -g -Wall
INCLUDES=-I./Includes -I../DataTypes/Includes
AR=ar
ARFLAGS=-cr
OFILES=date.o errors.o standard.o unicode.o path.o utils.o
LIBNAME=utils

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
