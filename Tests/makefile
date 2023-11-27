CC=clang
CFLAGS=-g -c -Wall -Wno-unused-function
INCLUDES= -I../Utils/Includes -I../DataTypes/Includes -I../Parser/Includes -I../Interp/Includes -I../Gedcom/Includes -I../Database/Includes
LIBLOCNS=-L../Utils/ -L../DataTypes/ -L../Parser/ -L../Interp -L../Gedcom -L../Database
LIBS=-lutils -lparser -ldatatypes -linterp -lgedcom -ldatabase

all: test hashtabletest stringtabletest testset

hashtabletest: testhashtable.o
	$(CC) -o hashtabletest testhashtable.o $(LIBLOCNS) $(LIBS) -lc

stringtabletest: teststringtable.o
	$(CC) -o stringtabletest teststringtable.o $(LIBLOCNS) $(LIBS) -lc

test: test.o ../Database/libdatabase.a ../Parser/libparser.a ../DataTypes/libdatatypes.a ../Interp/libinterp.a ../Gedcom/libgedcom.a
	$(CC) -o test test.o $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

testset: testset.o ../Database/libdatabase.a ../Parser/libparser.a ../DataTypes/libdatatypes.a ../Interp/libinterp.a ../Gedcom/libgedcom.a
	$(CC) -o testset testset.o $(INCLUDES) $(LIBLOCNS) $(LIBS) -lc

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $<
