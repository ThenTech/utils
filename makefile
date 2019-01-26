# Extra options for encoder compilation
# e.g.: global defines with -DNAME
ECFLAGS =

# Output folder for binaries
OUTPUT = ./bin

# Link libs such as: -pthread -lm -fopenmp
LIBS = -lstdc++fs

# Use a g++ executable with c++17 support (see install_g++.sh)
CC = g++-7

# Extra flags to strip unused symbols: -Wl,--strip-all,--gc-sections -fdata-sections -ffunction-sections
# Debug
# CFLAGS = $(ECFLAGS) -std=c++17 -Wall -Og
# Release
CFLAGS = $(ECFLAGS) -std=c++17 -Wall -O3 -Wl,--strip-all,--gc-sections -fdata-sections -ffunction-sections

# Default target
TARGET      = utils
TARGET_TEST = test_utils

# Tools
createout = @mkdir -p $(OUTPUT)
cleanobj  = @-rm -rf *.o

##################################################################

.PHONY: all default $(TARGET) test clean

all:
	@$(MAKE) --no-print-directory $(TARGET)
	@$(MAKE) --no-print-directory test

pre: clean
	$(createout)

default: pre compile
	$(cleanobj)
    
test: CFLAGS := -DENABLE_TESTS $(CFLAGS)
test: TARGET  = $(TARGET_TEST)
test: default
	@./$(OUTPUT)/$(TARGET_TEST)

$(TARGET): default

##################################################################

# Look for .hpp/.cpp files to compile and link
OBJECTS = $(patsubst ./%.cpp,./%.o, $(wildcard *.cpp **/*.cpp))
HEADERS = $(wildcard *.hpp)

# Compile each .cpp file to its object
%.o: %.cpp $(HEADERS)
	@echo "Compile" $@
	@$(CC) $(CFLAGS) $(CODE_DEF) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

# Call compiler for linking
compile: $(OBJECTS)
	@echo "Compiled and linking to" $(OUTPUT)/$(TARGET)
	@$(CC) $(OBJECTS) -Wall $(CFLAGS) $(LIBS) -o $(OUTPUT)/$(TARGET)

# Clean targets
cleantg:
	@-rm -r -f $(OUTPUT)/$(TARGET)
	@-rm -r -f $(OUTPUT)/$(TARGET_TEST)

# Clean all?
clean:
	$(cleanobj)
