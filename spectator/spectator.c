/*
 *spectator.c 
 *
 * 
 *
 *CS50, Winter 2022
 */


#include <stdio.h>
#include <stdlib.h>
#include "../support/message.h"
#include "../grid/grid.h"
#include "../libcs50/mem.h"

/**************** global types ****************/
typedef struct spectator {
  grid_t* grid;
  addr_t address;
} spectator_t;

/**************** global functions ****************/
/* that is, visible outside this file */
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);
void spectator_delete(spectator_t* spectator);

addr_t spectator_getAddress(spectator_t* spectator);
grid_t* spectator_getGrid(spectator_t* spectator);

void spectator_setAddress(spectator_t* spectator, addr_t address);
void spectator_setGrid(spectator_t* spectator, grid_t* grid);

/********** spectator_new **************/
/* see spectator.h for description */
spectator_t* spectator_new(grid_t* masterGrid, addr_t address)
{
  spectator_t* spectator = malloc(sizeof(spectator_t));
  if (spectator != NULL) {
    spectator->grid = masterGrid;
    spectator->address = address;
    return spectator;
  }

  fprintf(stderr, "Error, memory allocation error for spectator\n");
  return NULL;
}

/************ spectator_delete() ************/
/* see spectator.h for description */
void spectator_delete(spectator_t* spectator)
{
  if (spectator != NULL) {
    free(spectator);
  }
}

/******** spectator_getAddress ************/
/* see spectator.h for description */
addr_t spectator_getAddress(spectator_t* spectator)
{
  if(spectator != NULL) {
    return spectator->address;
  }
  return message_noAddr();
}

/******** spectator_getGrid ************/
/* see spectator.h for description */
grid_t* spectator_getGrid(spectator_t* spectator) 
{
  if (spectator != NULL) {
    return spectator->grid;
  }
  return NULL;
}


/******** spectator_setAddress ************/
/* see spectator.h for description */
void spectator_setAddress(spectator_t* spectator, addr_t address)
{
  if(spectator != NULL && message_isAddr(address)) {
    spectator->address = address;
  }
}

/******** spectator_setGrid ************/
/* see spectator.h for description */
void spectator_setGrid(spectator_t* spectator, grid_t* grid)
{
  if(spectator != NULL && grid != NULL) {
    spectator->grid = grid;
  }
}




