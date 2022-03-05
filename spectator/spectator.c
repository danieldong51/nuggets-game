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
  addr_t* address;
} spectator_t;

/**************** local functions ****************/
spectator_t* spectator_new(grid_t* masterGrid, addr_t address);
void spectator_delete(spectator_t* spectator);


/********get Address ************/
addr_t* getAddress(spectator_t* spectator){
  if(spectator != NULL) {
    return spectator->address;
  }
  return NULL;
}


/**********spectator_new**************/
spectator_t* spectator_new(grid_t* masterGrid, addr_t address){
  spectator_t* spectator = malloc(sizeof(spectator_t));
  if (spectator != NULL) {
    spectator->grid = masterGrid;
    spectator->address = &address;
    return spectator;
  }

  fprintf(stderr, "Error, memory allocation error for spectator\n");
  return NULL;
}


/************spectator_delete()************/
void spectator_delete(spectator_t* spectator){
  if (spectator != NULL) {
    free(spectator);
  }
}




