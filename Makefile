# Makefile for manipulationtools

# Executable

EXT	= 64
BASE01	= compose
EXE01	= $(BASE01)$(EXT)
VERSION = 1.0

TOOLS	= $(EXE01)

# Compiler stuff

CC	= gcc
CFLAGS	= -O3 -Wall -I$(LOCAL_LIB_PATH)/include
LIBS	= -L$(LOCAL_LIB_PATH)/lib -lm -liof

# Object definition

OBJ01	= $(BASE01).o

# Rules

all:	$(TOOLS)

$(EXE01): $(OBJ01) Makefile
	$(CC) $(CFLAGS) $(OBJ01) -o $(EXE01) $(LIBS)

clean:
	-rm -f *.o *~ $(TOOLS)

tar:
	cd ..; tar cvf - manipulationtools/*.c manipulationtools/*.h manipulationtools/Makefile > manipulationtools-$(VERSION).tar
