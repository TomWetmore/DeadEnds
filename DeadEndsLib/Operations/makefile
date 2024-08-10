CC=clang
CFLAGS=-c -g -Wall -Wno-unused-but-set-variable
INCLUDES=-I./Includes -I../Utils/Includes -I../DataTypes/Includes -I../Gedcom/Includes -I../Database/Includes
AR=ar
ARFLAGS=-cr
OFILES=addtofamily.o createfamily.o generatekey.o
LIBNAME=operations

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<

clean:
	rm *.o *.a
