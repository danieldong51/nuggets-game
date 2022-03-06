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
#include "../libcs50/mem.h"
#include "../grid/grid.h"
#include "../support/message.h"
#include "player.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/


/**************** global types ****************/
typedef struct player {
  char* name; 
  char letter; 
  int numGold; 
  grid_t* grid;
  addr_t address;
  bool isTalking; 
} player_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */

player_t* player_new();
void player_delete(player_t* player); 
bool player_isTalking(player_t* player);
int player_getGold(player_t* player);
grid_t* player_getGrid(player_t* player);
char* player_getName(player_t* player);
char player_getLetter(player_t* player);
addr_t player_getAddress(player_t* player);

// setter functions 
void player_addGold(player_t* player, int numGold);
void player_changeStatus(player_t* player, bool status);
void player_setLetter(player_t* player, char letter);
void player_setName(player_t* player, char* name);
void player_setGrid(player_t* player, grid_t* grid);
void player_setAddress(player_t* player, addr_t address); 

/**************** player_new() ****************/
/* see player.h for description */
player_t* player_new()
{
  // malloc memory for new player 
  player_t* player = mem_malloc_assert(sizeof(player_t), "Unable to allocate memory for player\n");

  // initialize attributes of player to nothing
  player->name = "";
  player->letter = ' ';
  player->numGold = 0; 
  player->grid = NULL;
  player->address = message_noAddr();
  player->isTalking = false; 

  return player; 
    
}

// GETTER FUNCTIONS

/**************** player_isTalking() ****************/
/* see player.h for description */
bool player_isTalking(player_t* player)
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
  return ' ';
}

/**************** player_getLetter() ****************/
/* see player.h for description */
addr_t player_getAddress(player_t* player)
{
  if (player != NULL) {
    return player->address; 
  }
  return message_noAddr();
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
  if (player != NULL && letter != ' ') {
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
void player_setAddress(player_t* player, const addr_t address)
{
  if (player != NULL && message_isAddr(address) == false) {
    player->address = address; 
  }
}

void player_delete(player_t* player)
{
  if (player != NULL) {
    // check if the grid object is NULL, free if not 
    if (player->grid != NULL) {
      gridDelete(player->grid);
    }
    
    mem_free(player);
  }
  
}