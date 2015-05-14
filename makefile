CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
OUTPUTFILE = kitty
B2PATH = parserscanner
B1PATH = symbol
OBJS = $(B1PATH)/*.o $(B2PATH)/*.o /*.o kitty.o 

$(OUTPUTFILE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(B1PATH)/*.o:
	cd $(@D) && $(MAKE)

$(B2PATH)/*.o:
	cd $(@D) && $(MAKE)

%.o: %.c
	$(CC) $(CFLAGS) -c $? -o $@

clean: 
	$(RM) $(OUTPUTFILE) $(OBJS) *~ && \
	cd $(B1PATH) && make clean && \
	cd ../$(B2PATH) && make clean

