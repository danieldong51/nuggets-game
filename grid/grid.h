#ifndef _GRID_H_
#define _GRID_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/bag.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include <math.h>

typedef struct position position_t;
typedef struct playerAndPosition playerAndPosition_t;
typedef struct pile pile_t;
typedef struct grid grid_t;


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
void updateGrid(grid_t* playerGrid, grid_t* masterGrid, char playerLetter);

/**************** gridPrint ****************/
/* Prints grid with locations of players and gold piles.
 *
 * Caller provides:
 *   a valid player grid struct and the playerLetter char.
 * We return:
 *   the char** representation of the grid that the player sees,
 *   with the visible players and gold piles.
 */
char* gridPrint(grid_t* playerGrid, char playerLetter);

/**************** gridValidMove ****************/ 
// NEED TO UPDATE!!
/* Checks if move is valid, and makes move if valid.
 *
 * Caller provides:
 *   a valid master map pointer, and a valid position pointer of the
 *   move it is trying to make.
 * We return:
 *   0 if the move is valid and the player moves to an empty space
 *   -1 if the move is invalid
 *   a positive integer of how much gold the player collects, if the player
 *   moves to a gold pile
 */
int gridValidMove(grid_t* masterGrid, char playerLetter, char moveLetter);

/**************** gridInit ****************/
/*
 */
grid_t* grid_new();

/**************** gridMakeMaster ****************/
void gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int randInt);

/**************** gridNewPlayer ****************/
/* Returns new playerGrid for player
 *
 * Caller provides:
 *   a valid master map pointer, and a valid char of player letter.
 * We return:
 *   NULL if error,
 *   a valid player map pointer otherwise, with initialized char**
 *   map.
 */
grid_t* gridNewPlayer(grid_t* masterGrid, char playerLetter);


/**************** getNumRows ****************/
int getNumRows(grid_t* masterGrid);


/**************** getNumColumns ****************/
int getNumColumns(grid_t* masterGrid);


/**************** getGrid2D ****************/
char** getGrid2D(grid_t* masterGrid);

void setPosition(position_t* position, int x, int y);


void gridDelete(grid_t* map);
void goldPilesDelete(pile_t** goldPiles);
void playerAndPositionDelete(playerAndPosition_t** playerPositions);
void grid_deletePlayer(grid_t* masterGrid, char playerLetter) ;

position_t* newPosition();
#endif