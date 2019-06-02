# Extra options for encoder compilation
# e.g.: global defines with -DNAME
ECFLAGS = 

# Output folder for binaries
OUTPUT      = ./bin
OUTPUT_GCOV = ./code_coverage

# Link libs such as: -pthread -lm -fopenmp
LIBS = -lstdc++fs

# Use a g++ executable with c++17 support
#   Install: 
#       sudo add-apt-repository ppa:ubuntu-toolchain-r/test
#       sudo apt update
#       sudo apt install g++-9 -y
#       sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-9 
# CXX = g++

# Extra flags to strip unused symbols: -Wl,--strip-all,--gc-sections -fdata-sections -ffunction-sections
# Debug
# CFLAGS = $(ECFLAGS) -std=c++17 -Wall -Og
# Release
CFLAGS = $(ECFLAGS) -std=c++17 -Wall -O3 -Wl,--strip-all,--gc-sections -fdata-sections -ffunction-sections

# Default target
TARGET      = utils
TARGET_TEST = test_utils
TARGET_GCOV = gcov_utils

# Tools
createout     = @mkdir -p $(OUTPUT) $(OUTPUT_GCOV)
cleanobj      = @-rm -rf *.o
cleancoverage = @-rm -rf *.gcov *.gcno *.gcda
ifndef fastcoverage
fastcoverage  = @fastcov
endif

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
    
coverage: CFLAGS := -DENABLE_TESTS -coverage -std=c++17 -Wall -O0
coverage: TARGET  = $(TARGET_GCOV)
coverage: default
	$(fastcoverage) --zerocounters
	@./$(OUTPUT)/$(TARGET_GCOV)
	$(fastcoverage) --exclude /usr/include utils_lib/external --lcov -o $(OUTPUT_GCOV)/report.info
	@genhtml -o $(OUTPUT_GCOV) $(OUTPUT_GCOV)/report.info
#	$(cleancoverage)

$(TARGET): default

##################################################################

# Look for .hpp/.cpp files to compile and link
OBJECTS = $(patsubst ./%.cpp,./%.o, $(wildcard *.cpp **/*.cpp))
HEADERS = $(wildcard *.hpp)

# Compile each .cpp file to its object
%.o: %.cpp $(HEADERS)
	@echo "Compile" $@
	@$(CXX) $(CFLAGS) $(CODE_DEF) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

# Call compiler for linking
compile: $(OBJECTS)
	@echo "Compiled and linking to" $(OUTPUT)/$(TARGET)
	@$(CXX) $(OBJECTS) -Wall $(CFLAGS) $(LIBS) -o $(OUTPUT)/$(TARGET)

# Clean targets
cleantg:
	@-rm -r -f $(OUTPUT)/$(TARGET)
	@-rm -r -f $(OUTPUT)/$(TARGET_TEST)
    
cleancov:
	@-rm -r -f $(OUTPUT_GCOV)

# Clean all?
clean:
	$(cleanobj)
	$(cleancoverage)
