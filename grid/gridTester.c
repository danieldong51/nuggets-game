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
  printf("rand int is %d\n", randInt);
  gridMakeMaster(masterGrid, "../maps/main.txt", 20, 10, 30, randInt);

  gridPrint(masterGrid, 'a');
  printf("%s", gridPrint(masterGrid, 'a'));
  
}