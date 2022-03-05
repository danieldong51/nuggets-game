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


/**************** local functions ****************/
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);
void spectator_delete(spectator_t* spectator);
grid_t* spectator_getGrid(spectator_t* spectator);
addr_t* spectator_getAddress(spectator_t* spectator);


/************ spectator_new ***********/
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);


/********** spectator_delete **********/
void spectator_delete(spectator_t* spectator);


/************ spectator_getAddress ***********/
addr_t* spectator_getAddress(spectator_t* spectator);

/******** spectator_getGrid ************/
grid_t* spectator_getGrid(spectator_t* spectator);



 

