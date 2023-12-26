###############################################################################
#	test_multithreading.cpp Makefile		    						      #
#																	          #
#	Project:	DeepShark        										      #
#	Date:		12/15/2023											          #
#																	          #
###############################################################################

## Binary filename
BINFILE = test_multithreading

## Compiler
CC=g++						# GNU C++ Compiler

## Directories
SRCDIR = ../src
OBJDIR = ../obj
INCDIR = ../inc
TESTINCDIR = ../tests/inc
BINDIR = ../bin
LIBDIR = ../lib
TESTDIR = ../tests/functional

## Compiler flags
CPPFLAGS = -I$(INCDIR)		# Look in inc directory for header files
#CPPFLAGS += -fpic			# Generate position independent code (for shared lib)
CPPFLAGS += -Wall			# Enables warnings for all questionable constructions
CPPFLAGS += -Wextra			# Enables additional warnings
CPPFLAGS += -Wconversion	# Warn for implicit conversions that may alter a value
CPPFLAGS += -Werror			# Make all warnings into errors
#CPPFLAGS += -g			# Enable debugging
#CPPFLAGS += -O0			# Disable most optimizations, use with debugging
CPPFLAGS += -std=c++23		# Enable support for ISO C++23 standard
CPPFLAGS += -O3				# Enable many optimizations
CPPFLAGS += -pthread		# Enable multithreading

## Define source, dependencies (headers), and object files
## Then append directory to the filenames
TEST = $(TESTDIR)/test_multithreading.cpp
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(addprefix $(OBJDIR)/, $(notdir $(SRC)))
OBJ += $(addprefix $(OBJDIR)/, $(notdir $(TEST)))
OBJ := $(OBJ:.cpp=.o)
DEPS = $(wildcard $(INCDIR)/*.hpp)

#SRC = $(wildcard $(SRCDIR)/*.cpp)
#OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))


# all:
# 	@echo $(SRC)
# 	@echo $(OBJ)
# Ensure make executes all rules
all: create_objdir $(BINDIR)/$(BINFILE)

## Create object directory if it does not exist
create_objdir:
	@mkdir -p $(OBJDIR)

## Create objects from source code
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) $(CPPFLAGS) -c $< -o $@

## Create objects from test code
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp $(DEPS)
	$(CC) $(CPPFLAGS) -c $< -o $@

## Create executable
$(BINDIR)/$(BINFILE): $(OBJ)
	$(CC) $(OBJ) -o $@ 

## Indicate phony targets
.PHONY: clean all 		# Runs rules even if files named "clean" or "all" exist

## Clean up
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~ $(BINDIR)/$(BINFILE)