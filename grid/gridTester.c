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
  // printf("made master grid\n");

  // printf("%s\n", gridPrint(masterGrid, 'a'));

  // printf("printed grid\n");

  
  // grid_t* playerAGrid = gridNewPlayer(masterGrid, 'a');
  // updateGrid(playerAGrid, masterGrid, 'a');
  // printf("%s\n", gridPrint(playerAGrid, 'a'));

  // grid_t* playerBGrid = gridNewPlayer(masterGrid, 'b');
  // updateGrid(playerBGrid, masterGrid, 'b');
  // printf("%s\n", gridPrint(playerBGrid, 'b'));

  // grid_t* playerCGrid = gridNewPlayer(masterGrid, 'c');
  // updateGrid(playerCGrid, masterGrid, 'c');
  // printf("%s\n", gridPrint(playerCGrid, 'c'));

  // grid_t* playerDGrid = gridNewPlayer(masterGrid, 'd');
  // updateGrid(playerDGrid, masterGrid, 'd');
  // printf("%s\n", gridPrint(playerDGrid, 'd'));

  // printf("created players\n");

  // updateGrid(playerAGrid, masterGrid, 'a');
  // printf("%s\n", gridPrint(playerAGrid, 'a'));

  // printf("updated playerA\n");

  // char c;
  // while (scanf("%c", &c) != NULL) {
  //   printf("return: %d\n", gridValidMove(masterGrid, 'a', c));
  //   updateGrid(playerAGrid, masterGrid, 'a');
  //   printf("%s\n", gridPrint(playerAGrid, 'a'));
  // }
  // gridDelete(playerAGrid);
  // gridDelete(playerBGrid);
  // gridDelete(playerCGrid);
  // gridDelete(playerDGrid);
  gridDelete(masterGrid);
}