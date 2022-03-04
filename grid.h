#ifndef _GRID_H_
#define _GRID_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct position position_t;
typedef struct playerAndPosition playerAndPosition_t;
typedef struct pile pile_t;
typedef struct grid grid_t;

/**************** newGrid2D ****************/
static char** newGrid2D(int NROWS, int NCOLS);


/**************** gridConvert ****************/
/*
 */
void gridConvert(char** grid, file *fp);

/**************** updateGrid ****************/
/* 
 */
void updateGrid(grid_t* playerGrid, grid_t* serverGrid);

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
grid_t* gridInit();

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

