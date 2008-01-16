# Makefile for manipulationtools

# Executable

EXT	= 64
BASE01	= compose
BASE02	= cutout
EXE01	= $(BASE01)$(EXT)
EXE02	= $(BASE02)$(EXT)
VERSION = 1.0

TOOLS	= $(EXE01) $(EXE02)

# Compiler stuff

CC	= gcc
CFLAGS	= -O3 -Wall -I$(LOCAL_LIB_PATH)/include
LIBS	= -L$(LOCAL_LIB_PATH)/lib -lm -liof

# Object definition

OBJ01	= $(BASE01).o
OBJ02	= $(BASE02).o

# Rules

all:	$(TOOLS)

$(EXE01): $(OBJ01) Makefile
	$(CC) $(CFLAGS) $(OBJ01) -o $(EXE01) $(LIBS)

$(EXE02): $(OBJ02) Makefile
	$(CC) $(CFLAGS) $(OBJ02) -o $(EXE02) $(LIBS)

clean:
	-rm -f *.o *~ $(TOOLS)

tar:
	cd ..; tar cvf - manipulationtools/*.c manipulationtools/*.h manipulationtools/Makefile > manipulationtools-$(VERSION).tar
