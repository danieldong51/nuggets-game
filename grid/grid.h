#ifndef _GRID_H_
#define _GRID_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bag.h"
#include "file.h"
#include "mem.h"
#include <math.h>

typedef struct position position_t;
typedef struct playerAndPosition playerAndPosition_t;
typedef struct pile pile_t;
typedef struct grid grid_t;

/**************** newGrid2D ****************/
static char** newGrid2D(int nrows, int ncols);


/**************** gridConvert ****************/
/* Converts map.txt file to a grid
 *
 * Caller provides:
 *   a malloc'd grid of size nrows x ncols
 *   a valid FILE pointer to a map.txt file
 *   integers nrows and ncols
 * We guarantee:
 *   the grid is filled with the map.txt file
 */
void gridConvert(char** grid, FILE* fp, int nrows, int ncols);

/**************** updateGrid ****************/
/* Updates player grid.
 *
 * Caller provides:
 *   a valid player struct pointer and a valid grid pointer.
 * We guarantee:
 *   player's grid is updated to add any new walls it sees.
 *   player's player and gold list is updated with any players
 *   or gold piles it sees currently.
 */
void updateGrid(player_t* player, grid_t* serverGrid);

/**************** gridPrint ****************/
/*
 */
void gridPrint(grid_t* grid, position_t* currentPosition);

/**************** gridValidMove ****************/
/*
 */
int gridValidMove(position_t* coordinate);

/**************** gridInit ****************/
/*
 */
grid_t* grid_new();

/**************** gridMakeMaster ****************/
void gridMakeMaster(grid_t* masterGrid, FILE* fp, int numGold, int minGoldPiles, int maxGoldPiles, int seed);

/**************** gridNewPlayer ****************/
grid_t* gridNewPlayer(grid_t* map);


/**************** getNumRows ****************/
int getNumRows(grid_t* masterGrid);


/**************** getNumColumns ****************/
int getNumColumns(grid_t* masterGrid);


/**************** getGrid2D ****************/
char** getGrid2D(grid_t* masterGrid);

#endif