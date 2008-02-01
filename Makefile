# Makefile for manipulationtools

# Executable

EXT	= 64
BASE01	= compose
BASE02	= cutout
BASE03	= rotate
BASE04	= extract
EXE01	= $(BASE01)$(EXT)
EXE02	= $(BASE02)$(EXT)
EXE03	= $(BASE03)$(EXT)
EXE04	= $(BASE04)$(EXT)
VERSION = 1.0

TOOLS	= $(EXE01) $(EXE02) $(EXE03) $(EXE04)

# Compiler stuff

CC	= gcc
CFLAGS	= -O3 -Wall -I$(LOCAL_LIB_PATH)/include
LIBS	= -L$(LOCAL_LIB_PATH)/lib -lm -liof

# Object definition

OBJ01	= $(BASE01).o
OBJ02	= $(BASE02).o
OBJ03	= $(BASE03).o
OBJ04	= $(BASE04).o

# Rules

all:	$(TOOLS)

$(EXE01): $(OBJ01) Makefile
	$(CC) $(CFLAGS) $(OBJ01) -o $(EXE01) $(LIBS)

$(EXE02): $(OBJ02) Makefile
	$(CC) $(CFLAGS) $(OBJ02) -o $(EXE02) $(LIBS)

$(EXE03): $(OBJ03) Makefile
	$(CC) $(CFLAGS) $(OBJ03) -o $(EXE03) $(LIBS)

$(EXE04): $(OBJ04) Makefile
	$(CC) $(CFLAGS) $(OBJ04) -o $(EXE04) $(LIBS)

clean:
	-rm -f *.o *~ $(TOOLS)

tar:
	cd ..; tar cvf - manipulationtools/*.c manipulationtools/*.h manipulationtools/Makefile > manipulationtools-$(VERSION).tar
