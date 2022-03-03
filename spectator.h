/*
 *
 * Team tux
 *
 * CS50, winter 2022
 *
 */


#include <stdio.h>
#include <stdlib.h>


//Message module
#include "message.h" 



//spectator struct
typedef struct spectator {
  grid_t grid;
} spectator_t;



/*
 * new_spectator
 */

spectator_t* spectator_new(grid_t* masterGrid);

/*
 *
 * delete_spectator
 *
 */
void spectator_delete(spectator_t* spectator);






 

