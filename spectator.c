/*
 *spectator.c 
 *
 * 
 *
 *CS50, Winter 2022
 */


#include <stdio.h>
#include <stdlib.h>
#include "message.h"
#include "spectator.h"
#include "grid.h"



/**************** local functions ****************/
/*
 *
 * spectator_new = allocates memory for spectator struct
 *
 */
spectator_t* spectator_new(grid_t* masterGrid){
  spectator_t* spectator = malloc(sizeof(spectator_t));
  if (spectator != NULL) {
    spectator->grid = masterGrid;
    return spectator;
  }

  flog_v(stderr, "Error, memory allocation error for spectator\n");
  return NULL;
}


/*
 *
 * spectator_delete = deletes a spectator instance
 *
 */
void spectator_delete(spectator_t* spectator){
  if (spectator != NULL) {
    free(spectator);
  }
}




