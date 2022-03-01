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
#include <time.h> 
#include "../lab-4-gmandell23/tse-crawler-gmandell23/libcs50/mem.h"


/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct position {
	int x;
	int y;
} position_t;


/**************** global types ****************/
typedef struct player {
  char* name; 
  char* letter; 
  position_t* position; 
  int numGold; 
  grid_t* grid;
  bool isTalking; 
} player_t;

typedef struct grid {
  char** grid2D;                    // 2d string array, each slot represents one row
  pile_t** goldPiles; 
  playerAndPosition_t** playerPositions;
} grid_t;

typedef struct playerAndPosition {
  char name;
  position_t* playerPosition;
} playerAndPosition_t;

typedef struct pile {
  position_t* location;
  int amount;
} pile_t;


/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
player_t* player_new(char* name, char* letter, grid_t* masterGrid);
void player_move(player_t* player, position_t* newPosition);
position_t* player_getPosition(player_t* player);
void player_addGold(player_t* player, int numGold);
void player_addGold(player_t* player, int numGold);
bool player_isTakling(player_t* player);
void player_changeStatus(player_t* player);
grid_t* player_getGrid(player_t* player);
char* player_getName(player_t* player);

/**************** player_new() ****************/
/* see player.h for description */
player_t* player_new(char* name, char* letter, grid_t* masterGrid)
{
  if (name != NULL && letter != NULL && masterGrid != NULL) {

    // allocate memory for a new player
    player_t* player = mem_malloc_assert(sizeof(player_t), "Unable to allocate memory for player object\n");

    // generate a random roomspot on the grid for the player to start

    int NR = sizeof(masterGrid)/sizeof(masterGrid[0]);                  // number of rows in grid 
    int NC = sizeof(masterGrid)/sizeof(masterGrid[1]);                  // number of columns in grid 
    char* spot = "";
    int x = 0;
    int y = 0; 

    // until we generate a spot that corresponds to an empty room spot (".")
    while (strcmp(spot, ".") != 0) {

      // generate a random x between 0 and NC, and a random y between 0 and NR 
      srand(time(NULL));          // initialize - do only one time
      x = rand() % NC;
      y = rand() % NR; 


      spot = masterGrid->grid2D[y][x];
    }
    if (x >= 0 && y >= 0){
      // once we have a valid room spot, set this as the position for the player
      player->position->x = x; 
      player->position->y = y; 

      // intialize an empty grid object for this player --> empty grid, empty piles, empty playerPositions
      grid_t* emptyGrid = grid_new(); 

      player->grid = emptyGrid; 
      player->isTalking = true; 
      
      player->numGold = 0;         // initialize this player's gold to 0 

      // set player's real name and server-given letter 
      player->name = name; 
      player->letter = letter; 

      return player; 
    }
    else {
      return NULL;
    }
    
    
  }
}

/**************** player_move() ****************/
/* see player.h for description */
void player_move(player_t* player, position_t* newPosition) 
{
  if (player != NULL && newPosition != NULL) {
    player->position = newPosition; 
  }
}


/**************** player_getPosition() ****************/
/* see player.h for description */
position_t* player_getPosition(player_t* player) 
{
  if (player != NULL) {
    return player->position; 
  }
  return NULL;
}

/**************** player_addGold() ****************/
/* see player.h for description */
void player_addGold(player_t* player, int numGold) 
{
  if (player != NULL && numGold > 0) {
    player->numGold += numGold; 
  }
}

/**************** player_isTalking() ****************/
/* see player.h for description */
bool player_isTakling(player_t* player)
{
  if (player != NULL) {
    return player->isTalking; 
  }
  return NULL;
}

/**************** player_changeStatus() ****************/
/* see player.h for description */
void player_changeStatus(player_t* player)
{
  if (player != NULL) {
    if (player_isTalking(player)){
      player->isTalking = false; 
    }
    else {
      player->isTalking = true; 
    }
  }
}

/**************** player_getGrid() ****************/
/* see player.h for description */
grid_t* player_getGrid(player_t* player)
{
  if (player != NULL){
    return player->grid; 
  }
  return NULL; 
}

/**************** player_getName() ****************/
/* see player.h for description */
char* player_getName(player_t* player) 
{
  if (player != NULL) {
    return player->name; 
  }
  return NULL;
}
