
muse_OBJS = muse.o
muse_PROGRAM = muse

PROGRAMS= $(muse_PROGRAM)
OBJS= $(muse_OBJS)

CC = gcc
LDFLAGS = -I/usr/include/ -L/usr/lib64/ -pthread -lfuse -lrt -ldl
CFLAGS = -Wall -O2 -D_FILE_OFFSET_BITS=64



.SUFFIXES: .c .o

all: $(muse_PROGRAM) 

$(muse_PROGRAM): $(muse_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

.c.o: 
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

.PHONY: clean
clean:
	-rm -rf $(PROGRAMS) $(OBJS)