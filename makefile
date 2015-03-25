CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
OUTPUTFILE = kitty
B3PATH = typechecker
B2PATH = parserscanner
B1PATH = symbol
DEPS = *.h $(B1PATH)/*.h $(B2PATH)/*.h $(B3PATH)/*.h
OBJS = kitty.o $(B1PATH)/*.o $(B2PATH)/*.o $(B3PATH)/*.o

$(OUTPUTFILE): $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) $^ -o $@

$(B1PATH)/*.o:
	cd $(@D) && $(MAKE)

$(B2PATH)/*.o:
	cd $(@D) && $(MAKE)

$(B3PATH)/*.o:
	cd $(@D) && $(MAKE)

%.o: %.c
	$(CC) $(CFLAGS) -c $? -o $@

clean: 
	$(RM) $(OUTPUTFILE) $(OBJS) *~
