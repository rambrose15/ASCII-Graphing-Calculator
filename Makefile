CXX = g++
# Compiler flags
CXX_FLAGS = -std=c++20 -Wall -g -O3 -Isrc
# Libraries to link (ncurses)
LIBS = -lncurses

SRC_DIR = src
OBJ_DIR = build
SOURCES = $(shell find $(SRC_DIR) -name '*.cc' -print)
OBJECTS = $(patsubst src/%.cc,$(OBJ_DIR)/%.o,$(SOURCES))

EXEC = $(OBJ_DIR)/graphing-calculator-program

$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC) $(CXX_FLAGS) $(LIBS)

# Compile .cc -> .o from src/ to build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@) 
	$(CXX) $(CXX_FLAGS) -c -o $@ $<


TEST_FLAGS = -std=c++20 -Wall -g -O0 -Isrc/model -Ilibs/doctest
TEST_DIR = tests
TEST_OBJ_DIR = $(OBJ_DIR)/test
TEST_SOURCES = $(shell find $(TEST_DIR) -name '*.cc' -print)
TEST_OBJECTS = $(patsubst tests/%.cc,$(TEST_OBJ_DIR)/%.o,$(TEST_SOURCES))
SRC_OBJECTS_FOR_TEST = $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS))
TEST_EXEC = $(TEST_OBJ_DIR)/test-suite

$(TEST_EXEC): $(TEST_OBJECTS) $(SRC_OBJECTS_FOR_TEST)
	$(CXX) $(TEST_OBJECTS) $(SRC_OBJECTS_FOR_TEST) -o $(TEST_EXEC) $(TEST_FLAGS) $(LIBS)

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cc
	@mkdir -p $(dir $@) 
	$(CXX) $(TEST_FLAGS) -c -o $@ $<

.PHONY: test
test: $(TEST_EXEC)
	./$(TEST_EXEC)
	
.PHONY: clean

clean:
	rm -rf build
	mkdir build
