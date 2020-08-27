# Extra options for encoder compilation
# e.g.: global defines with -DNAME
ECFLAGS = 

# Output folder for binaries
OUTPUT      = ./bin
OUTPUT_GCOV = ./docs/code_coverage
COV_IGNORE  = /usr/include utils_lib/external

# Link libs such as: -pthread -lm -fopenmp
LIBS = -lstdc++fs -pthread -lsqlite3

# Use a g++ executable with c++17 support
#   Install: 
#       sudo add-apt-repository ppa:ubuntu-toolchain-r/test
#       sudo apt update
#       sudo apt install g++-9 -y
#       sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-9 
#       pip3 install git+https://github.com/rpgillespie6/fastcov.git
# CXX = g++
# LD = ld

# Extra flags to strip unused symbols: -Wl,--strip-all,--gc-sections -fdata-sections -ffunction-sections
# Debug
# CFLAGS = $(ECFLAGS) -std=c++17 -Wall -Og
# Release
CFLAGS = $(ECFLAGS) -std=c++17 -Wall -O3 -Wl,--strip-all,--gc-sections -fdata-sections -ffunction-sections -Wno-unknown-pragmas

# Default target
TARGET      = utils
TARGET_TEST = test_utils
TARGET_GCOV = gcov_utils

# Tools
createout     = @mkdir -p $(OUTPUT) $(OUTPUT_GCOV)
cleanobj      = @-rm -rf *.o **/*.o
cleancoverage = @-rm -rf *.gcov *.gcno *.gcda **/*.gcov **/*.gcno **/*.gcda
ifndef fastcoverage
fastcoverage  = @fastcov
endif

CPUCOUNT = $(grep -c "^processor" /proc/cpuinfo)

##################################################################

.PHONY: all default $(TARGET) test multi clean

all:
	@$(MAKE) --no-print-directory $(TARGET)
	@$(MAKE) --no-print-directory test

multi:
	@$(MAKE) -j ${CPUCOUNT} --no-print-directory $(TARGET)

pre: clean
	$(createout)

default: pre compile
	$(cleanobj)
    
test: CFLAGS := -DENABLE_TESTS $(CFLAGS)
test: TARGET  = $(TARGET_TEST)
test: default
	@$(OUTPUT)/$(TARGET_TEST)
    
coverage: CFLAGS := -DENABLE_TESTS -coverage -std=c++17 -Wall -O0 -Wno-unknown-pragmas
coverage: TARGET  = $(TARGET_GCOV)
coverage: default
	$(fastcoverage) --zerocounters
	@$(OUTPUT)/$(TARGET_GCOV)
	$(fastcoverage) --exclude $(COV_IGNORE) --lcov -o $(OUTPUT_GCOV)/report.info
	@genhtml -o $(OUTPUT_GCOV) $(OUTPUT_GCOV)/report.info

$(TARGET): default

##################################################################

# Look for .hpp/.cpp files to compile and link
OBJECTS   = $(patsubst %.cpp,%.o, $(wildcard *.cpp **/*.cpp))
HEADERS   = $(wildcard *.hpp **/*.hpp)
RESOURCES = $(patsubst %.rc,%.o, $(wildcard *.rc **/*.rc))

# Compile each .cpp file to its object
%.o: %.cpp $(HEADERS)
	@echo "Compile" $< "->" $@
	@$(CXX) $(CFLAGS) $(CODE_DEF) -c $< -o $@

# Dump each .rc file to a binary object
%.o: %.rc
	@echo "Compile" $< "->" $@
	@$(LD) -r -b binary $< -o $@
ifeq ($(OS),Windows_NT)
	@shell command -v windres 1> /dev/null && windres $< -o $@
endif

.PRECIOUS: $(TARGET) $(OBJECTS) $(RESOURCES)

# Call compiler for linking
compile: $(OBJECTS) $(RESOURCES)
	@echo "Compiled and linking to" $(OUTPUT)/$(TARGET)
	@$(CXX) $(OBJECTS) -Wall $(CFLAGS) $(LIBS) $(RESOURCES) -o $(OUTPUT)/$(TARGET)

# Clean targets
cleantg:
	@-rm -r -f $(OUTPUT)/$(TARGET)
	@-rm -r -f $(OUTPUT)/$(TARGET_TEST)
	@-rm -r -f $(OUTPUT)/$(TARGET_GCOV)
    
cleancov:
	@-rm -r -f $(OUTPUT_GCOV)

# Clean all?
clean:
	$(cleanobj)
	$(cleancoverage)
