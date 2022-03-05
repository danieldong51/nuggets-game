# Makefile for 'grid' module
#
# Team Tux

OBJS = player.o ../grid/grid.o
LIBS = ../libcs50/libcs50-given.a  ../support/support.a 

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

player.o: player.h ../libcs50/libcs50-given.a ../grid/grid.h ../libcs50/mem.h ../support/message.h 

.PHONY: clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f player