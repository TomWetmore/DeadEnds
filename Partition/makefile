CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function
LL=../DeadEndsLib/
INCLUDES= -I$(LL)Database/Includes -I$(LL)DataTypes/Includes -I$(LL)Validate/Includes -I$(LL)Gedcom/Includes -I$(LL)Utils/Includes -I$(LL)Operations/Includes
LIBLOCNS=-L$(LL)Database -L$(LL)DataTypes -L$(LL)Gedcom -L$(LL)Utils -L$(LL)Validate
LIBS= -ldatabase -lvalidate -lgedcom -ldatatypes -lutils

partition: main.o connect.o
	$(CC) -o partition main.o connect.o  $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

clean:
	rm -f *.o partition

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
