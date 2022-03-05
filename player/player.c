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
<<<<<<< HEAD
#include "mem.h"
#include "../grid/grid.h"
=======
#include "libcs50/mem.h"
#include "grid.h"
#include "../support/message.h"
>>>>>>> 26210d46943172e3bcc22c6dd4f1b2941b8b8c92

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/


/**************** global types ****************/
typedef struct player {
  char* name; 
  char letter; 
  int numGold; 
  grid_t* grid;
  addr_t* address;
  bool isTalking; 
} player_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
bool player_isTakling(player_t* player);
int player_getGold(player_t* player);
grid_t* player_getGrid(player_t* player);
char* player_getName(player_t* player);
char player_getLetter(player_t* player);
addr_t* player_getAddress(player_t* player);

// setter functions 
void player_addGold(player_t* player, int numGold);
void player_changeStatus(player_t* player, bool status);
void player_setLetter(player_t* player, char letter);
void player_setName(player_t* player, char* name);
void player_setGrid(player_t* player, grid_t* grid);
void player_setAddress(player_t* player, addr_t* address); 

/**************** player_new() ****************/
/* see player.h for description */
player_t* player_new(char* name, char* letter, grid_t* masterGrid)
{
  if (name != NULL && letter != NULL && masterGrid != NULL) {

    // allocate memory for a new player
    player_t* player = mem_malloc_assert(sizeof(player_t), "Unable to allocate memory for player object\n");

    // TODO: change the name of this function to grid_new() 
    player->grid = grid_new();                 // malloc memory for grid - do not fill 

    return player; 
  }
  else {
    return NULL;
  }
    
}

// GETTER FUNCTIONS

/**************** player_isTalking() ****************/
/* see player.h for description */
bool player_isTakling(player_t* player)
{
  if (player != NULL) {
    return player->isTalking; 
  }
  return NULL;
}

/**************** player_getGold() ****************/
/* see player.h for description */
int player_getGold(player_t* player)
{
  if (player != NULL) {
    return player->numGold;
  }
  return -1; 
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

/**************** player_getLetter() ****************/
/* see player.h for description */
char player_getLetter(player_t* player)
{
  if (player != NULL) {
    return player->letter; 
  }
  return NULL;
}

/**************** player_getLetter() ****************/
/* see player.h for description */
addr_t* player_getAddress(player_t* player)
{
  if (player != NULL) {
    return player->address; 
  }
  return NULL;
}

// SETTER FUNCTIONS

/**************** player_addGold() ****************/
/* see player.h for description */
void player_addGold(player_t* player, int numGold) 
{
  if (player != NULL && numGold > 0) {
    player->numGold += numGold; 
  }
}

/**************** player_changeStatus() ****************/
/* see player.h for description */
void player_changeStatus(player_t* player, bool status)
{
  if (player != NULL) {
    player->isTalking = status; 
  }
}

/**************** player_setName() ****************/
/* see player.h for description */
void player_setName(player_t* player, char* name)
{
  if (player != NULL && name != NULL) {
    player->name = name; 
  }
}

/**************** player_setLetter() ****************/
/* see player.h for description */
void player_setLetter(player_t* player, char letter)
{
  if (player != NULL && letter != NULL) {
    player->letter = letter; 
  }
}

/**************** player_setGrid() ****************/
/* see player.h for description */
void player_setGrid(player_t* player, grid_t* grid)
{
  if (player != NULL && grid != NULL) {
    player->grid = grid; 
  }
}

/**************** player_setLetter() ****************/
/* see player.h for description */
void player_setAddress(player_t* player, addr_t* address)
{
  if (player != NULL && address != NULL) {
    player->address = address; 
  }
}
