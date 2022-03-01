#include <stdio.h>
#include <stdlib.h>
#include "bag.h"

typedef struct position {
  int x;
  int y;
} position_t;

typedef struct playerAndPosition {
  char name;
  position_t* playerPosition;
} playerAndPosition_t;

typedef struct pile {
  position_t* location;
  int amount;
} pile_t;

typedef struct grid {
  char** grid2D;                    // 2d string array, each slot represents one row
  pile_t** goldPiles; 
  playerAndPosition_t** playerPositions;
} grid_t;


/**************** gridConvert ****************/
void gridConvert(char** grid, FILE *fp) 
{
// dan is doing
}

/**************** updateGrid ****************/
bag_t* updateGrid(grid_t* playerGrid, grid_t* serverGrid)
{
// dan is doing
}

/**************** gridPrint ****************/
void gridPrint(grid_t* map, position_t* currentPosition)
{
  int players = sizeof(map->playerPositions)/sizeof(map->playerPositions[0]);                   // determine number of players that are to be printed
  
  // set the players in the grid array
  for (int i = 0, i < players, i++) {
    position_t* tempPosition = mem_malloc(sizeof(position_t));
    tempPosition = map->playerPositions[i];
    // print position of the current player
    if ( (tempPosition->x == currentPosition->x) && (tempPosition->y == currentPosition->y)) {    
      map[currentPosition->x][currentPosition->y] = '@';
    }
    // print position of the other players 
    else {
      map[tempPosition->x][tempPosition->y] = grid->platerPositions[i]->name;
    }
  }
  mem_free(tempPosition);

  // set the gold in the grid array
  int numPiles = sizeof(map->goldPiles)/sizeof(map->goldPiles[0]);                  
  for ( i = 0; i< numPiles; i++ ) {
    position_t* tempPosition = mem_malloc(sizeof(position_t));
    tempPosition = map->goldPiles[i]->location;
    map[tempPosition->x][tempPosition->y] = '*';
  }

  // calculate amount of rows in grid
  int NR = sizeof(map->grid)/sizeof(map->grid[0]);

  // print out each row
  for ( i = 0; i < NR; i ++ ) {
    fprintf(stdout, "%s", map->grid[i]);
  }
}

/**************** gridValidMove ****************/
int gridValidMove(position_t* coordinate)
{
  return 0;
}