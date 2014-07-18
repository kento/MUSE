*_OBJS = *.o
*_PROGRAM = *

PROGRAMS= $(*_PROGRAM)
OBJS= $(*_OBJS)

CC = gcc
LDFLAGS = -I/usr/include/ -L/usr/lib64/
CFLAGS = -Wall -O2

.SUFFIXES: .c .o

all: $(*_PROGRAM) 

$(*_PROGRAM): $(*_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

.c.o: 
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

.PHONY: clean
clean:
	-rm -rf $(PROGRAMS) $(OBJS)