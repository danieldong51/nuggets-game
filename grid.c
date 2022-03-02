#include <stdio.h>
#include <stdlib.h>
#include "bag.h"

typedef struct position {
  int x;
  int y;
} position_t;

typedef struct playerAndPosition {
  char name;
  position_t* playerPosition;
} playerAndPosition_t;

typedef struct pile {
  position_t* location;
  int amount;
} pile_t;

typedef struct grid {
  char** grid2D;                    // 2d string array, each slot represents one row
  pile_t** goldPiles; 
  playerAndPosition_t** playerPositions;
} grid_t;

static int NROWS = 25;
static int NCOLS = 100;

static int ROCK = ' ';
static int EMPTY = '.';
static int PASSAGE = '#';
static int HORIZONTAL = '-';
static int VERITAL = '|';
static int CORNER = '+';

/**************** newGrid ****************/
static char**
newGrid(void)
{
  // allocate a 2-dimensional array of NROWS x NCOLS
  char** grid = calloc(NROWS, sizeof(char*));
  char* contents = calloc(NROWS * NCOLS, sizeof(char));
  if (grid == NULL || contents == NULL) {
    fprintf(stderr, "cannot allocate memory for map\r\n");
    exit(1);
  }

  // set up the array of pointers, one for each row
  for (int y = 0; y < NROWS; y++) {
    grid[y] = contents + y * NCOLS;
  }

  // fill the board with empty cells
  for (int y = 0; y < NROWS; y++) {
    for (int x = 0; x < NCOLS; x++) {
      grid[y][x] = ' ';
    }
  }
  return grid;
}

static void
girdConvert(char** grid, FILE* fp)
{
  const int size = NCOLS+2;  // include room for \n\0
  char line[size];           // a line of input
  int y = 0;

  // read each line and copy it to the board
  while ( fgets(line, size, fp) != NULL && y < NROWS) {
    int len = strlen(line);
    if (line[len-1] == '\n') {
      // normal line
      len--; // don't copy the newline
    } else {
      // overly wide line
      len = NCOLS;
      fprintf(stderr, "board line %d too wide for screen; truncated.\r\n", y);
      for (char c = 0; c != '\n' && c != EOF; c = getc(fp))
        ; // scan off the excess part of the line
    }
    strncpy(grid[y++], line, len);
  }

  if (!feof(fp)) {
    fprintf(stderr, "board too big for screen; truncated to %d lines\r\n", y);
  }
}

/**************** updateGrid ****************/
bag_t* updateGrid(grid_t* playerGrid, grid_t* serverGrid)
{
// dan is doing
}

/**************** gridPrint ****************/
void gridPrint(grid_t* map, position_t* currentPosition)
{
  int players = sizeof(map->playerPositions)/sizeof(map->playerPositions[0]);                   // determine number of players that are to be printed
  
  // set the players in the grid array
  for (int i = 0, i < players, i++) {
    position_t* tempPosition = mem_malloc(sizeof(position_t));
    tempPosition = map->playerPositions[i];
    // print position of the current player
    if ( (tempPosition->x == currentPosition->x) && (tempPosition->y == currentPosition->y)) {    
      map[currentPosition->x][currentPosition->y] = '@';
    }
    // print position of the other players 
    else {
      map[tempPosition->x][tempPosition->y] = grid->platerPositions[i]->name;
    }
  }
  mem_free(tempPosition);

  // set the gold in the grid array
  int numPiles = sizeof(map->goldPiles)/sizeof(map->goldPiles[0]);                  
  for ( i = 0; i< numPiles; i++ ) {
    position_t* tempPosition = mem_malloc(sizeof(position_t));
    tempPosition = map->goldPiles[i]->location;
    map[tempPosition->x][tempPosition->y] = '*';
  }

  // calculate amount of rows in grid
  int NR = sizeof(map->grid)/sizeof(map->grid[0]);

  // print out each row
  for ( i = 0; i < NR; i ++ ) {
    fprintf(stdout, "%s", map->grid[i]);
  }
}

/**************** gridValidMove ****************/
int gridValidMove(position_t* coordinate)
{
  return 0;
}