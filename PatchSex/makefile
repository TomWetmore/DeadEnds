CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function
LL=../DeadEndslib/
INCLUDES= -I$(LL)/Includes -I$(LL)Database/Includes -I$(LL)DataTypes/Includes -I$(LL)Gedcom/Includes -I$(LL)Interp/Includes -I$(LL)Operations/Includes -I$(LL)Parser/Includes -I$(LL)Utils/Includes -I$(LL)Validate/Includes
LIBLOCNS=-L$(LL)Database -L$(LL)DataTypes -L$(LL)Gedcom -L$(LL)Interp -L$(LL)Operations -L$(LL)Parser -L$(LL)Utils -L$(LL)Validate
LIBS=-ldatabase -ldatatypes -lgedcom -linterp -loperations -lparser -lutils -lvalidate

patchsex: patchsex.o
	$(CC) -o patchsex patchsex.o  $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

clean:
	rm -f *.o patchsex

%.o: %.c
	echo echo $(INCLUDES)
	$(CC) $(CFLAGS) $(INCLUDES) $<
