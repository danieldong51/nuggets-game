#!/bin/bash
#
#
# Author: Team Tux 
# CS50, Winter 2022, Nuggets
#
# Server testing script 

#./server ../maps/main.txt

# nonexistent map file 
./server ../maps/error.txt

# invalid # arguments: no arguments
./server 

# invalid seed : string 
./server ../maps/main.txt ../maps/error.txt

# invalid seed : non integer 
./server ../maps/main.txt -1

# invalid # arguments: 3 arguments 
./server ../maps/main.txt ../maps/error.txt d






