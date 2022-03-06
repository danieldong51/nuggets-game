// unit testing for grid

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bag.h"
#include "file.h"
#include "mem.h"
#include <math.h>
#include "grid.h"


int 
main(const int argc, char* argv[])
{

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

  printf("line 29\n");

  char** grid2D = getGrid2D(masterGrid);
  int NR = getNumRows(masterGrid);

  for (int i = 0; i < NR; i++ ) {
    printf("%d\n", i);
    printf("%s\n", grid2D[i]);
  }

  printf("line 36\n");

  // Reads main.txt mao
  FILE* mapFile = fopen("../maps/main.txt", "r");
  gridConvert(grid2D, mapFile, getNumRows(masterGrid), getNumColumns(masterGrid));  

  printf("line 42\n");

  gridPrint(masterGrid, 'a');

  
}