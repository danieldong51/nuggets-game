# Makefile for 'grid' module
#
# Emily Gao

OBJS = player.o
LIBS = ../libcs50/libcs50-given.a 

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

player: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


player.o: player.h ../libcs50/libcs50-given.a ../grid/grid.h ../libcs50/mem.h ../support/message.h

.PHONY: clean

# expects a file `hashtableFile.txt` to exist; it can contain any text.

# expects a file `hashtableFile.txt` to exist; it can contain any text.
#valgrind: gridTester ../maps/small.txt
#$(VALGRIND) ./gridTester

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f player
