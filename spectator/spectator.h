/*
 *
 * Team tux
 *
 * CS50, winter 2022
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include "../grid/grid.h"
#include "../support/message.h" 
#include "../libcs50/mem.h"

/**************** global types ****************/
typedef struct spectator spectator_t;

/**************** global functions ****************/
/* that is, visible outside this file */
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);
void spectator_delete(spectator_t* spectator);

addr_t spectator_getAddress(spectator_t* spectator);
grid_t* spectator_getGrid(spectator_t* spectator);

void spectator_setAddress(spectator_t* spectator, addr_t address);
void spectator_setGrid(spectator_t* spectator, grid_t* grid);


/**************** spectator_new() ****************/
/* A function to create a new spectator struct. */
/* 
* Caller provides: 
*   the address of the spectator and the masterGrid
* We do: 
*   allocate memory for a new spectator 
*   set the spectators address to be the address
* We return: 
*   the spectator, if everything is successful
*   NULL, on error
*/
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);


/**************** spectator_delete() ****************/
/* A function to delete a spectator struct and free all of its memory */
/*
* We do: 
*   if the player is not null:
*     free spectator 
* We return: 
*   nothing
*/
void spectator_delete(spectator_t* spectator);


/**************** spectator_getLetter() ****************/
/* A function to return the address of a spectator*/
/* 
* Caller provides: 
*   a spectator object
* We return: 
*   the spectators's address, if the spectator is not null 
*    null otherwise 
*/addr_t spectator_getAddress(spectator_t* spectator);

/******** spectator_getGrid ************/
/* A function to return the grid object of a spectator*/
/* 
* Caller provides: 
*   a spectator object
* We return: 
*   the spectator's grid object, if the spectator is not null 
*    null otherwise 
*/
grid_t* spectator_getGrid(spectator_t* spectator);

/**************** spectator_setAddress() ****************/
/* A function to change the aessddr of a spectator */
/* 
* Caller provides: 
*   a spectator object
*   an addr_t object
* We do: 
*   change the spectator's address to this address, as long as the given spectator and address aren't NULL
*/
void spectator_setAddress(spectator_t* spectator, addr_t address);

/**************** spectator_setGrid() ****************/
/* A function to change the grid of a spectator */
/* 
* Caller provides: 
*   a spectator object 
*   a grid_t object
* We do: 
*   change the spectator's grid to this grid, as long as the given spectator and grid aren't NULL
*/
void spectator_setGrid(spectator_t* spectator, grid_t* grid);

