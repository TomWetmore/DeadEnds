CC=clang
CFLAGS=-c -g -Wall -Wno-unused-function -Wno-unused-but-set-variable
INCLUDES=-I./Includes -I../DataTypes/Includes -I../Gedcom/Includes -I../Utils/Includes -I../Database/Includes -I../Validate/Includes
AR=ar
ARFLAGS=-cr
OFILES=validate.o valperson.o valfamily.o
LIBNAME=validate

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
