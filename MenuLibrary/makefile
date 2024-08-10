CC=clang
CFLAGS=-c -g -Wall -Wno-unused-function
INCLUDES=-I./Includes -I../DeadEndsLib/Utils/Includes -I../DeadEndsLib/DataTypes/Includes
AR=ar
ARFLAGS=-cr
OFILES=ask.o menu.o
LIBNAME=menulib

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<

clean:
	rm *.o *.a
