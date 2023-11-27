.PRECIOUS: grammar.c

CC=clang
CFLAGS=-c -g -Wall
INCLUDES=-I. -I./Includes -I../DataTypes/Includes -I../Gedcom/Includes -I../Interp/Includes -I../Database/Includes -I../Utils/Includes
AR=ar
ARFLAGS=-cr
OFILES=parse.o lexer.o grammar.o
LIBNAME=parser

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<

%.c: %.y
	oyacc -d $<
	cp -f y.tab.c $@
