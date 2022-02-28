/* 
 * player.c - Nuggets 'player' module
 * 
 * see player.h for more information
 * 
 * CS50, Winter 2022 
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include "grid.h"


/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct position {
	int x;
	int y;
} position_t*;


/**************** global types ****************/
typedef struct player {
  char* name; 
  position_t* position; 
  int numGold; 
  grid_t* grid;
  bool isTalking; 
} player_t*;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
player_t* player_new(char* name, grid_t* masterGrid)
{
  if (name != NULL) {
    // generate a random roomspot on the grid for the player to start

    // call visibility on player 
  }
}
