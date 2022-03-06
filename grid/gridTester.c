// unit testing for grid

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bag.h"
#include "file.h"
#include "mem.h"
#include <math.h>
#include "../player/player.h"
#include "grid.h"

void tester();
static char** newGrid2D(int nrows, int ncols);

int 
main(const int argc, char* argv[])
{
  tester();
}

  printf("line start\n");

  grid_t* masterGrid = grid_new();
  //printf("made grid\n");
  char* fileName; 
  fileName = argv[1];

  printf("line 1\n");

  //FILE* fp = fopen(fileName, "r");
  //char line[15];
  //fgets(line, 15, fp);
  //printf("tester line: %s\n", line);
  gridMakeMaster(masterGrid, fileName, 10, 1 ,5, 1);
  printf("made master\n");

  FILE* mapFile = fopen("../maps/challenge.txt", "r");

  printf("nrow is %d, ncol is %d\n", nrow, ncol);
  gridConvert(grid2D, mapFile, nrow, ncol);

  for (int i = 0; i < nrow; i++) {
    for (int j = 0; j < ncol; j++) {
      printf("%c", grid2D[i][j]);
    }
    printf("\n");
  }
}

static char**
newGrid2D(int nrows, int ncols)
{
  // allocate a 2-dimensional array of nrows x ncols
  char** grid = calloc(nrows, sizeof(char*));
  char* contents = calloc(nrows * ncols, sizeof(char));
  if (grid == NULL || contents == NULL) {
    fprintf(stderr, "cannot allocate memory for map\r\n");
    exit(1);
  }

  // set up the array of pointers, one for each row
  for (int y = 0; y < nrows; y++) {
    grid[y] = contents + y * ncols;
  }

  // fill the board with empty cells
  for (int y = 0; y < nrows; y++) {
    for (int x = 0; x < ncols; x++) {
      grid[y][x] = ' ';
    }
  }
  return grid;
}