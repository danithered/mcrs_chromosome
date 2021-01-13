PROGNAME=simulation

IDIR =./src/include
ODIR=./src/obj
SRCDIR=./src

CC=g++
C=gcc

CFLAGS=-I$(IDIR) `pkg-config --cflags gsl` `pkg-config --cflags RNAlib2` -ggdb -fexceptions -Wall # for testing
#CFLAGS=-I$(IDIR) `pkg-config --cflags gsl` `pkg-config --cflags RNAlib2` -O3 # for stuff

LIBS=-lm `pkg-config --libs gsl` `pkg-config --libs RNAlib2`

_DEPS = ca.h randomgen.h dv_tools.h parameters.h rnarep.h annot.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o ca.o dv_tools.o parameters.o rnarep.o annot.o randomgen.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ_test = test.o annot.o parameters.o dv_tools.o bitmuveletek.o 
OBJ_test = $(patsubst %,$(ODIR)/%,$(_OBJ_test))


$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(C) -c -o $@ $< $(CFLAGS)

$(PROGNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~  

.PHONY: run

run:
	./$(PROGNAME)  

test: $(OBJ_test)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	./test

