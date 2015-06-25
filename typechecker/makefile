CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
DEPS = ./*.h
OBJS = kittyweed.o kittymulticollect.o kittycollect.o \
	stack.o kittycheck.o

all: $(OBJS) $(DEPS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	$(RM) *.o
