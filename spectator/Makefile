# Makefile for 'spectator' module
#
# Team Tux 

OBJS = spectator.o ../grid/grid.o
LIBS = ../libcs50/libcs50-given.a 

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

spectator.o: spectator.h ../libcs50/libcs50-given.a ../grid/grid.h ../libcs50/mem.h ../support/message.h

.PHONY: clean


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f spectator
