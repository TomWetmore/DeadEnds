CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function
LL=../DeadEndslib/
INCLUDES= -I$(LL)Database/Includes -I$(LL)DataTypes/Includes -I$(LL)Gedcom/Includes -I$(LL)Interp/Includes -I$(LL)Operations/Includes -I$(LL)Parser/Includes -I$(LL)Utils/Includes -I$(LL)Validate/Includes -I../MenuLibrary/Includes
LIBLOCNS=-L$(LL)Database -L$(LL)DataTypes -L$(LL)Gedcom -L$(LL)Interp -L$(LL)Operations -L$(LL)Parser -L$(LL)Utils -L$(LL)Validate -L../MenuLibrary
LIBS=-ldatabase -ldatatypes -lgedcom -linterp -loperations -lparser -lutils -lvalidate -lmenulib

usemenus: main.o personmenu.o
	$(CC) -o usemenus main.o personmenu.o $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

clean:
	rm *.o usemenus

%.o: %.c
	echo echo $(INCLUDES)
	$(CC) $(CFLAGS) $(INCLUDES) $<
