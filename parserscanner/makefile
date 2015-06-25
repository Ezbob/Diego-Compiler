CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
DEPS = memory.h kittytree.h y.tab.h 
OBJS = lex.yy.o kittytree.o kittyprinter.o y.tab.o memory.o
SCAN = kittyscanner.l
PARS = kittyparser.y

all: lex.yy.c y.tab.c $(OBJS) $(DEPS)

%.o: %.c
	$(CC) $(CFLAGS) -c $? -o $@

lex.yy.c: $(SCAN)
	flex $<

y.tab.c y.tab.h: $(PARS)
	bison -dy $<

clean: 
	$(RM) lex.yy.c y.tab.h y.tab.c *.o