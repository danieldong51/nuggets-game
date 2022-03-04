/* 
 * player.h - header file for Nuggets 'player' module
 * 
 * A 'player' is a module providing the data structure to represent a player in the Nuggets game. 
 * Each player keeps track of its own name, location, the amount of gold it has, 
 * the rooms it has seen before (its personal grid struct), and whether the player is currently talking to the server.
 *
 * CS50, Winter 2022 
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "../grid/grid.h"


/**************** global types ****************/
typedef struct player player_t; 

/**************** global functions ****************/
/* that is, visible outside this file */

/**************** local functions ****************/
/* not visible outside this file */
player_t* player_new(char* name, char* letter, grid_t* masterGrid);

// getter functions 
bool player_isTakling(player_t* player);
int player_getGold(player_t* player);
grid_t* player_getGrid(player_t* player);
char* player_getName(player_t* player);
char player_getLetter(player_t* player);

// setter functions 
void player_addGold(player_t* player, int numGold);
void player_changeStatus(player_t* player);
void player_setLetter(player_t* player, char letter);
void player_setName(player_t* player, char* name);
void player_setGrid(player_t* player, grid_t* grid);

/**************** player_new() ****************/
/* A function to create a new player struct. */
/* 
* Caller provides: 
*   the real name of the player, its letter, and the masterGrid
* We do: 
*   allocate memory for a new player 
*   choose a random room spot for the player 
*   initialize information about the player 
* We return: 
*   the player, if everything is successful, 
*   NULL, on error
*/
player_t* player_new(char* name, char* letter, grid_t* masterGrid);


/**************** player_isTakling() ****************/
/* A function to return whether or not the player is talking to the server. */
/* 
* Caller provides: 
*   a player object
* We return: 
*   if the player is not null, return whether or not the player is talking to the server 
*/
bool player_isTakling(player_t* player);

/**************** player_getGold() ****************/
/* A function to return the amount of gold a player has */
/* 
* Caller provides: 
*   a player object
* We return: 
*   if the player is not null, the amount of gold this player has 
*   if the player is null, return -1  
*/
int player_getGold(player_t* player);

/**************** player_changeStatus() ****************/
/* A function to return the grid object of a player*/
/* 
* Caller provides: 
*   a player object
* We return: 
*   the player's grid object, if the player is not null 
*    null otherwise 
*/
grid_t* player_getGrid(player_t* player);

/**************** player_getName() ****************/
/* A function to return the real name of a player*/
/* 
* Caller provides: 
*   a player object
* We return: 
*   the player's real name, if the player is not null 
*    null otherwise 
*/
char* player_getName(player_t* player);

/**************** player_getLetter() ****************/
/* A function to return the assigned letter of a player*/
/* 
* Caller provides: 
*   a player object
* We return: 
*   the player's letter, if the player is not null 
*    null otherwise 
*/
char player_getLetter(player_t* player);


/**************** player_addGold() ****************/
/* A function to add to the gold count of a player. */
/* 
* Caller provides: 
*   a player object, an integer representing the amount of gold to add
* We do: 
*   if the player is not null, add numGold amount of gold to the player's numGold attribute 
* We return: 
*   nothing
*/
void player_addGold(player_t* player, int numGold);

/**************** player_changeStatus() ****************/
/* A function to change whether or not the player is talking to the server. */
/* 
* Caller provides: 
*   a player object
* We do: 
*   call player_isTalking to see if the player is currently talking to the server 
*   if it is: 
*     change the isTalking boolean to false 
*   if it is not: 
*     change the isTalking boolean to true 
*/
void player_changeStatus(player_t* player);

/**************** player_setName() ****************/
/* A function to change the name of a player */
/* 
* Caller provides: 
*   a player object
* We do: 
*   change the player's name to this string
* We do not need to check if the name is in the range of MaxPlayerNameLength, the server does this 
*/
void player_setName(player_t* player, char* name);

/**************** player_setLetter() ****************/
/* A function to change the letter of a player */
/* 
* Caller provides: 
*   a player object
* We do: 
*   change the player's letter to this char
*  
*/
void player_setLetter(player_t* player, char letter);


/**************** player_setGrid() ****************/
/* A function to change the grid of a player */
/* 
* Caller provides: 
*   a grid_t object
* We do: 
*   change the player's grid to this grid, as long as the given player and grid aren't NULL
*/
void player_setGrid(player_t* player, grid_t* grid);