#include directories
IFLAGS=-I. -I../../include

#C compiler
CC=gcc

#C++ compiler
CCC=g++

#C compiler flags
ifeq ($(CONFIG), debug)
CFLAGS=-c -Wall $(IFLAGS)
else
CFLAGS = -pipe -O2 -DNDEBUG $(IFLAGS) 
endif

#C++ compiler flags
ifeq ($(CONFIG), debug)
CCFLAGS=-g
else
CCFLAGS=-g -O2 -DNDEBUG
endif

#link flags
LDFLAGS = 

#library folder
ifeq ($(CONFIG), debug)
LIB = ../../lib/linux/debug
else
LIB = ../../lib/linux/release
endif

#source files

CSRC = ../common/qcwildcardtest.c \
../common/qcsohashtest.c

CCSRC = ../../src/qc/qcintf.cpp \
../common/wildcardtest.cpp \
../common/sohashtest.cpp \
../common/qcpptest.cpp

CCOBJS=$(CCSRC:.cpp=.o)
COBJS=$(CSRC:.c=.o)

TESTEXE=qcpptest.out

LIBDEP=../src/qc/qcintf.o

all: $(TESTEXE)

$(TESTEXE) : $(LIBOUT) $(TESTOBJS)
	$(CC) $(LDFLAGS) $(LIBOUT) $(TESTOBJS) -o $@

$(LIBOUT): $(LIBDEP)
	ar rcs $(LIBOUT) $(LIBDEP)
	
$(LIBDEP) : ccbuild

$(TESTOBJS) : ccbuild cbuild

ccbuild : $(CCOBJS)

$(CCOBJS):  $(CCSRC)
	$(CCC) $(CCFLAGS) -c $< -o $@
	
cbuild : $(COBJS)
	
$(COBJS): $(CSRC)
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
