CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function -Wno-unused-variable
LL=../DeadEndslib/
INCLUDES= -I$(LL)Database/Includes -I$(LL)DataTypes/Includes -I$(LL)Gedcom/Includes -I$(LL)Interp/Includes -I$(LL)Operations/Includes -I$(LL)Parser/Includes -I$(LL)Utils/Includes -I$(LL)Validate/Includes
LIBLOCNS=-L$(LL)Database -L$(LL)DataTypes -L$(LL)Gedcom -L$(LL)Interp -L$(LL)Operations -L$(LL)Parser -L$(LL)Utils -L$(LL)Validate
LIBS=-lparser -linterp -loperations -ldatabase -lvalidate -lgedcom -ldatatypes -lutils

gensexpr: main.o sexpr.o
	$(CC) -o gensexpr  main.o sexpr.o  $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

clean:
	rm -f *.o gensexpr

%.o: %.c
	echo echo $(INCLUDES)
	$(CC) $(CFLAGS) $(INCLUDES) $<
