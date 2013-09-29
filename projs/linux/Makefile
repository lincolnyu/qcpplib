#configurations
ifneq ($(CONFIG), debug)
CONFIG=release
endif

#include directories
IFLAGS=-I. -I../../include

#C compiler
CC=gcc

#C++ compiler
CCC=g++

#C compiler flags
ifeq ($(CONFIG),debug)
CFLAGS=-c -Wall $ifLAGS)
else
CFLAGS = -pipe -O2 -DNDEBUG $(IFLAGS)
endif

#C++ compiler flags
ifeq ($(CONFIG),debug)
CCFLAGS=-g $ifLAGS)
else
CCFLAGS=-g -O2 -DNDEBUG $(IFLAGS)
endif

#link flags
LDFLAGS=

#library folder
ifeq ($(CONFIG),debug)
LIB = ../../lib/linux/debug
else
LIB = ../../lib/linux/release
endif

# C source code for testing
TESTCSRC=../common/qcwildcardtest.c \
../common/qcsohashtest.c
	
# C++ source code for testing
TESTCCSRC=../common/wildcardtest.cpp \
../common/sohashtest.cpp \
../common/qcpptest.cpp

# All source code for testing
TESTSRC=$(TESTCSRC) $(TESTCCSRC)

# C source code for libraries
LIBCSRC=

# C++ source code for libraries
LIBCCSRC=../../src/qc/qcintf.cpp

# All source code for the libraries
LIBSRC=$(LIBCSRC) $(LIBCCSRC)

# All C source code
CSRC=$(TESTCSRC)

# All C++ source code
CCSRC=$(TESTCCSRC) $(LIBCCSRC)

# Binaries built from C source code for testing
TESTCOBJS=$(TESTCSRC:.c=.o)

# Binaries built from C++ source code for testing
TESTCCOBJS=$(TESTCCSRC:.cpp=.o)

# Binaries built from all testing source code
TESTOBJS=$(TESTCOBJS) $(TESTCCOBJS)

# Binaries built from C source code for the libraries
LIBCOBJS=$(LIBCSRC:.c=.o)

# Binaries built from C++ source code for the libraries
LIBCCOBJS=$(LIBCCSRC:.cpp=.o)

# Binaries built from all souce code for the libraries
LIBOBJS=$(LIBCOBJS) $(LIBCCOBJS)

# Binaries built from all C source code
COBJS=$(CSRC,.c=.o)

# Binaries built from all C++ source code
CCOBJS=$(CCSRC,.cpp=.o)

# The tesing executable
TESTEXE=../../bin/linux/$(CONFIG)/qcpptest.out

# The objects the C interface libarary depends on
LIBDEP=../../src/qc/qcintf.o

# The C interface library
LIBOUT=../../lib/linux/libqcintf.a

# All static libraries
LIBS=$(LIBOUT)

# All executables
EXES=$(TESTEXE)

# Build all source code and the testing program
.PHONY: all
all: $(TESTEXE)

# Build testing executable
$(TESTEXE) : libcpl testcpl
	$(CCC) $(LDFLAGS) $(LIBOBJS) $(TESTOBJS) -o $@

# Build the static library
.PHONY: libbuild
libbuild: $(LIBOUT)

# Create an archived (static) library
$(LIBOUT): $(LIBOBJS)
	ar rcs $(LIBOUT) $(LIBOBJS)

# Compile files for libraries
libcpl: $(LIBCOBJS) $(LIBCCOBJS)

# Compile files for testing
testcpl: $(TESTCOBJS) $(TESTCCOBJS)

# C file compilation
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
	
# C++ file compilation
.cpp.o:
	$(CCC) $(CCFLAGS) -c $< -o $@
	
.PHONY: clean
clean: objclean libclean execlean
	
objclean: 
	rm -f $(TESTOBJS)
	
libclean:
	rm -f $(LIBS)
	
execlean:
	rm -f $(EXES)

