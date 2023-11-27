CC=clang
CFLAGS=-c -g -Wall -Wno-unused-function -Wno-unused-but-set-variable
INCLUDES=-I./Includes -I../DataTypes/Includes -I../Gedcom/Includes -I../Utils/Includes
AR=ar
ARFLAGS=-cr
OFILES=database.o nameindex.o recordindex.o import.o validate.o
LIBNAME=database

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
