CUDD=../cudd-install

IDIR=$(CUDD)/include
LDIR=$(CUDD)/lib

BISON=bison
FLEX=flex
CC=g++

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/sgrk
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC)) $(OBJ_DIR)/sgrk_parser.o $(OBJ_DIR)/sgrk_scanner.o

CPPFLAGS := -I$(IDIR) -MMD -MP
CFLAGS := -Wall --std=c++17
LDFLAGS := -L$(LDIR)
LDLIBS := -lcudd

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp: $(SRC_DIR)/%.yy
	$(BISON) $(BISONFLAGS) -o $(SRC_DIR)/$*.cpp $<

$(SRC_DIR)/%.cpp: $(SRC_DIR)/%.ll
	$(FLEX) $(FLEXFLAGS) -o $@ $<

$(OBJ_DIR)/sgrk_parser.o: $(SRC_DIR)/sgrk_parser.hpp
$(OBJ_DIR)/sgrk_scanner.o: $(SRC_DIR)/sgrk_parser.hpp
$(OBJ_DIR)/Driver.o: $(SRC_DIR)/sgrk_parser.hpp

clean:
	@$(RM) -rv $(EXE) $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d

-include $(OBJ:.o=.d)
