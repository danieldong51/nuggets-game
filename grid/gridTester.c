// unit testing for grid

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bag.h"
#include "file.h"
#include "mem.h"
#include <math.h>
#include "grid.h"

void tester();

int 
main(const int argc, char* argv[])
{
  tester();
}

void
tester() 
{
  // initiailizing gridMaster;
  grid_t* masterGrid = grid_new();
  printf("initialized masterGrid\n");

  srand(2);
  int randInt = rand();
  gridMakeMaster(masterGrid, "../maps/main.txt", 20, 10, 30, 1);
  printf("made master grid\n");

  printf("%s\n", gridPrint(masterGrid, 'a'));

  gridPrint(masterGrid, 'a');
  printf("%s", gridPrint(masterGrid, 'a'));

  printf("printed grid\n");

  grid_t* playerAGrid = gridNewPlayer(masterGrid, 'a');

  printf("created playerA grid\n");
  printf("%s", gridPrint(masterGrid, 'a'));
  printf("\n\n now gridValidMove\n\n");
  char c;
  while (( c = getchar()) != 'q'){
      printf("%d\n", gridValidMove(masterGrid, 'a', c));
      printf("%s", gridPrint(masterGrid, 'a'));
  }
  
}