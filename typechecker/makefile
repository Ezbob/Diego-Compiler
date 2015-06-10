CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
DEPS = ./*.h
OBJS = kittyweed.o kittyprinter.o kittytype.o stack.o kittycheck.o kittysettype.o

all: $(OBJS) $(DEPS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	$(RM) *.o
