CC=clang
CFLAGS=-c -g -Wall -Wno-unused-but-set-variable
INCLUDES=-I./Includes -I../Utils/Includes -I../DataTypes/Includes -I../Database/Includes
AR=ar
ARFLAGS=-cr
OFILES=gedcom.o gnode.o lineage.o name.o nodeutls.o readnode.o splitjoin.o writenode.o
LIBNAME=gedcom

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
