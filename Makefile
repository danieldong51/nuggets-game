# Makefile for CS50 Nuggets
# CS50, Winter 2022
# Team Tux

L = libcs50
.PHONY: all clean

############## default: make all libs and programs ##########
# If libcs50 contains set.c, we build a fresh libcs50.a;
# otherwise we use the pre-built library provided by instructor.
all: 
	(cd $L && if [ -r set.c ]; then make $L.a; else cp $L-given.a $L.a; fi)
	make -C support
	make -C grid
	make -C player
	make -C spectator
	make -C server
	

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C libcs50 clean
	make -C server clean
	make -C grid clean
	make -C player clean
	make -C spectator clean
	make -C support clean

