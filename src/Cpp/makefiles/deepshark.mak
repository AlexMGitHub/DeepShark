###############################################################################
#	deepshark.cpp Makefile					    						      #
#																	          #
#	Project:	DeepShark        										      #
#	Date:		12/26/2023											          #
#																	          #
###############################################################################

## Binary filename
LIBFILE = lib_deepshark.so

## Compiler
CC=g++						# GNU C++ Compiler

## Directories
SRCDIR = ../src
OBJDIR = ../obj
INCDIR = ../inc
BINDIR = ../bin
LIBDIR = ../lib

## Compiler flags
CPPFLAGS = -I$(INCDIR)		# Look in inc directory for header files
CPPFLAGS += -fpic			# Generate position independent code (for shared lib)
CPPFLAGS += -Wall			# Enables warnings for all questionable constructions
CPPFLAGS += -Wextra			# Enables additional warnings
CPPFLAGS += -Wconversion	# Warn for implicit conversions that may alter a value
CPPFLAGS += -Werror			# Make all warnings into errors
#CPPFLAGS += -g				# Enable debugging
#CPPFLAGS += -O0			# Disable most optimizations, use with debugging
CPPFLAGS += -std=c++23		# Enable support for ISO C++23 standard
CPPFLAGS += -O3				# Enable many optimizations
CPPFLAGS += -pthread		# Enable multithreading

## Define source, dependencies (headers), and object files
## Then append directory to the filenames
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
all: create_objdir $(LIBDIR)/$(LIBFILE)

## Create object directory if it does not exist
create_objdir:
	@mkdir -p $(OBJDIR)

## Create objects from source code
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) $(CPPFLAGS) -c $< -o $@

## Create shared library
$(LIBDIR)/$(LIBFILE): $(OBJ)
	$(CC) $(OBJ) -shared -o $@ 

## Indicate phony targets
.PHONY: clean all 		# Runs rules even if files named "clean" or "all" exist

## Clean up
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~ $(LIBDIR)/$(LIBFILE)