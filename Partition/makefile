CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function
LL=../DeadEndsLib/
INCLUDES= -I$(LL)/Includes -I$(LL)Database/Includes -I$(LL)DataTypes/Includes -I$(LL)Validate/Includes \
	-I$(LL)Parser/Includes -I$(LL)Gedcom/Includes -I$(LL)Utils/Includes -I$(LL)Operations/Includes -I$(LL)Interp/Includes
LIBLOCNS=-L$(LL)Database -L$(LL)DataTypes -L$(LL)Gedcom -L$(LL)Utils -L$(LL)Validate
LIBS= -ldatabase -lvalidate -lgedcom -ldatatypes -lutils

partition: main.o connect.o partition.o
	$(CC) -o partition main.o connect.o partition.o $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

clean:
	rm -f *.o partition

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
