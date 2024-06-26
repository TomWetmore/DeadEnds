CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function
INCLUDES= -I../Utils/Includes -I../DataTypes/Includes -I../Parser/Includes -I../Interp/Includes -I../Gedcom/Includes -I../Database/Includes -I../Validate/Includes
LIBLOCNS=-L../Utils/ -L../DataTypes/ -L../Parser/ -L../Interp -L../Gedcom -L../Database -L../Validate -L../Operations
LIBS=-lutils -lparser -ldatatypes -linterp -lgedcom -ldatabase -lvalidate -loperations

all: test


test: test.o testsequence.o testgedcomstrings.o testwritedatabase.o importone.o ../Database/libdatabase.a ../Parser/libparser.a ../DataTypes/libdatatypes.a ../Interp/libinterp.a ../Gedcom/libgedcom.a ../Validate/libvalidate.a
	$(CC) -o test test.o testsequence.o testgedcomstrings.o testwritedatabase.o importone.o  $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

testset: testset.o ../Database/libdatabase.a ../Parser/libparser.a ../DataTypes/libdatatypes.a ../Interp/libinterp.a ../Gedcom/libgedcom.a
	$(CC) -o testset testset.o $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
