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
CFLAGS = $(ECFLAGS) -std=c++17 -Wall -Og
# Release
# CFLAGS = $(ECFLAGS) -std=c++17 -Wall -O3 -Wl,--strip-all,--gc-sections -fdata-sections -ffunction-sections

# Default target
TARGET = test_utils

# Tools
createout = @mkdir -p $(OUTPUT)
cleanobj  = @-rm -f *.o

##################################################################

.PHONY: all default $(TARGET) clean

all:
	@$(MAKE) --no-print-directory $(TARGET)

pre: clean
	$(createout)

default: pre compile
	$(cleanobj)

$(TARGET): default

##################################################################

# Look for .hpp/.cpp files to compile and link
OBJECTS = $(patsubst %.cpp,%.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.hpp)

# Compile each .cpp file to its object
%.o: %.cpp $(HEADERS)
	@$(CC) $(CFLAGS) $(CODE_DEF) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

# Call compiler for linking
compile: $(OBJECTS)
	@$(CC) $(OBJECTS) -Wall $(CFLAGS) $(LIBS) -o $(OUTPUT)/$(TARGET)

# Clean targets
cleantg:
	@-rm -r -f $(OUTPUT)/$(TARGET)

# Clean all?
clean:
	$(cleanobj)
