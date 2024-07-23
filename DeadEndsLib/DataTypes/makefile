CC=clang
CFLAGS=-c -g -Wall -Wno-unused-function
INCLUDES=-I./Includes -I../Utils/Includes
AR=ar
ARFLAGS=-cr
OFILES=list.o hashtable.o sort.o set.o stringtable.o integertable.o block.o stringset.o
LIBNAME=datatypes

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
