#ifndef _GRID_H_
#define _GRID_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct position position_t;
typedef struct playerAndPosition playerAndPosition_t;
typedef struct pile pile_t;
typedef struct grid grid_t;

/**************** gridConvert ****************/
/*
 */
void gridConvert(char** grid, file *fp);

/**************** updateGrid ****************/
/* 
 */
bag_t* updateGrid(grid_t* playerGrid, grid_t* serverGrid);

/**************** gridPrint ****************/
/*
 */
void gridPrint(grid_t* grid, position_t* currentPosition);

/**************** gridValidMove ****************/
/*
 */
int gridValidMove(position_t* coordinate);