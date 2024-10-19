CC=clang
CFLAGS=-c -g -Wall -Wno-unused-but-set-variable -Wno-missing-braces -O0
INCLUDES=-I./Includes -I../Utils/Includes -I../DataTypes/Includes -I../Gedcom/Includes -I../Database/Includes
AR=ar
ARFLAGS=-cr
OFILES= builtin.o builtintable.o evaluate.o functable.o functiontable.o interp.o intrpevent.o intrpfamily.o intrpgnode.o \
        intrpmath.o intrpperson.o intrpseq.o pnode.o pvalue.o pvaluetable.o sequence.o symboltable.o builtinlist.o rassa.o \
	intrpstring.o
LIBNAME=interp

lib$(LIBNAME).a: $(OFILES)
	$(AR) $(ARFLAGS) lib$(LIBNAME).a $(OFILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<

clean:
	rm *.o *.a
