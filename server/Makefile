CC = cc 
CFLAGS = -Wall -Werror
PROG_NAME = s

SRCDIR = src

SOURCES := $(shell find $(SRCDIR) -name '*.c')
OBJECTS := $(SOURCES:%.c=%.o)

.PHONY: clean

xmod: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(OBJECTS)

clean:
	rm -f $(PROG_NAME) $(OBJECTS)