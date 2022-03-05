/*
 *
 * Team tux
 *
 * CS50, winter 2022
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "support/message.h" 

/**************** global types ****************/
typedef struct spectator spectator_t;


/**************** local functions ****************/
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);
void spectator_delete(spectator_t* spectator);

//getter
addr_t* getAddress(spectator_t* spectator);


/************spectator_new***********/
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);


/**********spectator_delete**********/
void spectator_delete(spectator_t* spectator);






 

