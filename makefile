PROGNAME=simulation

IDIR =./src/include
ODIR=./src/obj
SRCDIR=./src

CC=g++

CFLAGS=-I$(IDIR) `pkg-config --cflags gsl` `pkg-config --cflags RNAlib2` -ggdb

LIBS=-lm `pkg-config --libs gsl` `pkg-config --libs RNAlib2`

_DEPS = ca.h randomgen.h dv_tools.h parameters.h rnarep.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o ca.o dv_tools.o parameters.o rnarep.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ_test = test.o 
OBJ_test = $(patsubst %,$(ODIR)/%,$(_OBJ_test))


$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

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

