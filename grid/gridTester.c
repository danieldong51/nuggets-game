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


int 
main(const int argc, char* argv[])
{

  grid_t* masterGrid = grid_new();
  //printf("made grid\n");
  char* fileName; 
  fileName = argv[1];
  //FILE* fp = fopen(fileName, "r");
  //char line[15];
  //fgets(line, 15, fp);
  //printf("tester line: %s\n", line);
  gridMakeMaster(masterGrid, fileName, 10, 1 ,3, 1);
  //printf("made master\n");
  char** grid2D = getGrid2D(masterGrid);
  int NR = getNumRows(masterGrid);

  for (int i = 0; i < NR; i++ ) {
    printf("%d\n", i);
    printf("%s\n", grid2D[i]);
  }

  // Reads main.txt mao
  FILE* mapFile = fopen("../maps/main.txt", "r");
  gridConvert(grid2D, mapFile, getNumRows(masterGrid), getNumColumns(masterGrid));

  // Creating new player
  player_t* playerA = player_new("tempName", "a", masterGrid);

  gridPrint(masterGrid, 'a');
  


  
}
