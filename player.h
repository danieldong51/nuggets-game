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
#include "libcs50/mem.h"
#include "grid.h"


/**************** global types ****************/
typedef struct player player_t; 

/**************** global functions ****************/
/* that is, visible outside this file */

/**************** local functions ****************/
/* not visible outside this file */
player_t* player_new(char* name, char* letter, grid_t* masterGrid);
void player_move(player_t* player, position_t* newPosition);
position_t* player_getPosition(player_t* player);
void player_addGold(player_t* player, int numGold);
bool player_isTakling(player_t* player);
void player_changeStatus(player_t* player);
grid_t* player_getGrid(player_t* player);
char* player_getName(player_t* player);


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

/**************** player_move() ****************/
/* A function to change the position of a player. */
/* 
* Caller provides: 
*   a player object and a position object representing the new position of the player 
* We do: 
*   if the player is not null, set the position of the player to the given position 
* We return: 
*   nothing 
*/
void player_move(player_t* player, position_t* newPosition);

/**************** player_getPosition() ****************/
/* A function to return the position of a player. */
/* 
* Caller provides: 
*   a player object 
* We return: 
*   if the player is not null, return the position of this player 
*
*/
position_t* player_getPosition(player_t* player);

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

/**************** player_isTakling() ****************/
/* A function to return whether or not the player is talking to the server. */
/* 
* Caller provides: 
*   a player object
* We return: 
*   if the player is not null, return whether or not the player is talking to the server 
*/
bool player_isTakling(player_t* player);

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
