CUDD=../cudd-install

IDIR=$(CUDD)/include
LDIR=$(CUDD)/lib
ODIR=.

BISON=bison
FLEX=flex
CC=g++
CFLAGS=-I$(IDIR) -L$(LDIR) --std=c++17

LIBS=-lcudd

_DEPS=
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=sgrk_parser.o sgrk_scanner.o CycleCover.o CycleStrategy.o Driver.o Main.o MemorylessStrategy.o PermissiveStrategy.o SeparationGrkPlayer.o SeparationGrkSolver.o SeparationGrkStrategy.o SpaceConnectivity.o TestSet.o Util.o VarMgr.o WeakFGGame.o WeakFGGameSolver.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

%.cpp %.hpp: %.yy
	$(BISON) $(BISONFLAGS) -o $*.cpp $<

%.cpp: %.ll
	$(FLEX) $(FLEXFLAGS) -o $@ $<

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sgrk: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

sgrk_parser.o: sgrk_parser.hpp
sgrk_scanner.o: sgrk_parser.hpp
Driver.o: sgrk_parser.hpp

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ sgrk
