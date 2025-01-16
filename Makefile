CXX = g++
# Compiler flags
CXXFLAGS = -std=c++20 -Wall -g -O3 -Isrc
# Libraries to link (ncurses)
LDLIBS = -lncurses

SRCDIR = src
OBJDIR = build
SOURCES = $(shell find $(SRCDIR) -name '*.cc' -print)
OBJECTS = $(patsubst src/%.cc,$(OBJDIR)/%.o,$(SOURCES))

EXEC = ${OBJDIR}/graphingCalculatorProgram

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC} ${CXXFLAGS} ${LDLIBS}

# Compile .cc -> .o from src/ to build/
$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(dir $@) 
	${CXX} ${CXXFLAGS} -c -o $@ $<

.PHONY: clean

clean:
	rm -f ${OBJECTS} ${EXEC}
