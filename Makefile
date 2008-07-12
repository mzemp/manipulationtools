# Names

NAME	= manipulationtools
TOOLS	= compose cutout rotate extract scaleas
EXT	= 64
VERSION = 1.0

# Compiler stuff

CC	= gcc
CFLAGS	= -O3 -Wall -I$(LOCAL_LIB_PATH)/include
LIBS	= -L$(LOCAL_LIB_PATH)/lib -lm -liof

# Rules

all:    $(TOOLS)

.c:
	$(CC) $(CFLAGS) $@.c $(LIBS) -o $@
ifneq ($(EXT),)
	mv $@ $@$(EXT)
endif

clean:
	-rm -f *.o *~
ifneq ($(EXT),)
	-rm *$(EXT)
else
	-rm $(TOOLS)
endif

tar:
	cd ..; tar cvf - $(NAME)/Makefile $(NAME)/*.c $(NAME)/*.h > $(NAME)-$(VERSION).tar

