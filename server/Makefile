# Makefile for CS50 'crawler' module 
#
# Georgia Mandell 
# CS50, Winter 2022, Lab 4 
# TSE Crawler 

# object files, and the target library
OBJS = server.o 
L = ../libcs50

CC = gcc
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
LIBS = -lm 
LLIBS = ../libcs50/libcs50.a ../support/support.a

all: server

# executable depends on object files 
server: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LLIBS) -o server
# Dependencies: object files depend on header files

server.o: ../libcs50/file.h ../support/message.h ../support/log.h ../player/player.h ../grid/grid.h ../spectator/spectator.h 

test: 
	bash -v ./testing.sh


.PHONY: all clean test 

# list all the sources and docs in this directory.
# (this rule is used only by the Professor in preparing the starter kit)
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f $(LIB) *~ *.o
	rm -f server
	rm -f core
