# Makefile for CS50 nuggests

# object files, and the target library
OBJS = server.o ../support/message.o ../support/log.o ../player/player.o ../grid/grid.o ../spectator/spectator.o
LIBS = ../libcs50/libcs50-given.a

CC = gcc
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50


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
