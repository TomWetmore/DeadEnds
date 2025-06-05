CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function -Wno-unused-variable
LL=../DeadEndslib/
INCLUDES= -I$(LL)/Includes -I$(LL)Database/Includes -I$(LL)DataTypes/Includes -I$(LL)Gedcom/Includes -I$(LL)Interp/Includes -I$(LL)Operations/Includes -I$(LL)Parser/Includes -I$(LL)Utils/Includes -I$(LL)Validate/Includes
LIBLOCNS=-L$(LL)Database -L$(LL)DataTypes -L$(LL)Gedcom -L$(LL)Interp -L$(LL)Operations -L$(LL)Parser -L$(LL)Utils -L$(LL)Validate
LIBS=-ldatabase -ldatatypes -lgedcom -linterp -loperations -lparser -lutils -lvalidate

testprogram: test.o testsequence.o testgedcomstrings.o testwritedatabase.o importone.o testgedpath.o $(LL)/Database/libdatabase.a $(LL)/Parser/libparser.a $(LL)/DataTypes/libdatatypes.a $(LL)/Interp/libinterp.a $(LL)/Gedcom/libgedcom.a $(LL)/Validate/libvalidate.a
	$(CC) -o testprogram test.o testsequence.o testgedcomstrings.o testwritedatabase.o importone.o testgedpath.o $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<

clean:
	rm -f *.o testprogram

