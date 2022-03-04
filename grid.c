#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

typedef struct player {
  int index;
  char* name;
  int numGold;
  grid_t* grid;
  bool isTalking;
} player_t;

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
newGrid2D(void)
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
gridConvert(char** grid, FILE* fp)
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
void updateGrid(player_t* player, grid_t* masterGrid)
{
  int playerIndex = player->name;
  position_t* playerPos = masterGrid->playerPositions[playerIndex];
  grid_t* playerGrid = player->grid;

  // initialize toVisit bag and visited grid
  bag_t* toVisit = bag_new();
  char** visited = newGrid();

  // clear players and piles of gold in playerGrid

  // add current squares to toVisit bag and mark as visited 
  position_t* curPosition = masterGrid->playerPositions[playerIndex]->playerPosition;
  bag_add(toVisit, curPosition);

  gridMark(visited, curPosition, '.');
  
  // while toVisit is not empty
  position_t* position;
  while ((position = bag_extract(toVisit)) != NULL) {

    // if the position is visible
    if (isVisible(playerPos, position, masterGrid)) {

      // mark square as visible
      gridMark(visited, curPosition, '.');

      // mark player grid
      gridMark(playerGrid, position, gridGetChar(masterGrid, position));

      // loop over adjacent squares
      bag_t* adjacentSquares = bag_new();

      int x = position->x;
      int y = position->y;

      position_t* positionLeft = position_new(x - 1, y);
      position_t* positionRight = position_new(x + 1, y);
      position_t* positionUp = position_new(x, y + 1);
      position_t* positionDown = position_new(x, y - 1);

      bag_add(adjacentSquares, positionLeft);
      bag_add(adjacentSquares, positionRight);
      bag_add(adjacentSquares, positionUp);
      bag_add(adjacentSquares, positionDown);

      position_t* positionAdjacent;

      while ((positionAdjacent = bag_extract(adjacentSquares)) != NULL) {

        // if square not visited
        if (gridGetChar(visited, positionAdjacent) != '.') {

          // mark square as visitied
          gridMark(visited, positionAdjacent, '.');

          // add to toVisit
          bag_add(toVisit, positionAdjacent);
        }
      }
    }
  }
}

char gridGetChar(char** grid, position_t* position)
{
  return grid[position->y][position->x];
}

void gridMark(char** grid, position_t* position, char mark) 
{
  grid[position->y][position->x] = mark;
}

bool isVisible(position_t* playerPos, position_t* square, grid_t* masterGrid)
{
  
}

position_t* position_new(int x, int y)
{
  position_t* position = malloc(sizeof(position_t));
  position->x = x;
  position->y = y;
  return position;
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
      map->grid2D[currentPosition->x][currentPosition->y] = '@';
    }
    // print position of the other players 
    else {
      map->grid2D[tempPosition->x][tempPosition->y] = grid->platerPositions[i]->name;
    }
  }
  mem_free(tempPosition);

  // set the gold in the grid array
  int numPiles = sizeof(map->goldPiles)/sizeof(map->goldPiles[0]);                  
  for ( int i = 0; i< numPiles; i++ ) {
    position_t* tempPosition = mem_malloc(sizeof(position_t));
    tempPosition = map->goldPiles[i]->location;
    map->grid2D[tempPosition->x][tempPosition->y] = '*';
  }

  // calculate amount of rows in grid
  int NR = sizeof(map->grid)/sizeof(map->grid[0]);

  // print out each row
  for ( int i = 0; i < NR; i ++ ) {
    fprintf(stdout, "%s", map->grid2D[i]);
  }
}

/**************** gridValidMove ****************/
/* gives an (x,y) position and checks to see if a player can move into that position in the map */
int gridValidMove(grid_t* map, position_t* coordinate)
{
  int xCord = coordinate->x;
  int yCord = coordinate->y;

  // if coordinate is an empty room space or passage
  if ( (map->grid2D[yCord][xCord] == EMPTY) || (map->grid2D[yCord][xCord]) == PASSAGE) ||  (map->grid2D[yCord][xCord]) == PASSAGE) {
    return 0;
  }

  // if the coordinate is a pile of gold
  else if ((map->grid2D[yCord][xCord] == '*')) {
    int numPiles = sizeof(map->goldPiles)/sizeof(map->goldPiles[0]);  
    for (int i = 0; i < numPiles, i++) {
      // loop through and find the pile structure with the same position
      if ((map->goldPiles[i]->location->x) == xCord) && (map->goldPiles[i]->location->y) == yCord) {
        return map->goldPiles->amount;
      }
    }
  }
  else {
    // not a valid space to move into
    return 0;
  }
}

/**************** gridInit ****************/
// initializes a new empty grid--mallocs memory
grid_t* gridInit(){
  grid_t* map = mem_malloc(sizeof(grid_t));
  return map;
}

/**************** gridMakeMaster ****************/
/* fill up char** array and piles_t** array */
void gridMakeMaster(grid_t* masterGrid, FILE* fp, int numGold, int minGoldPiles, int maxGoldPiles, int seed) {
  char** grid2D;                                              // map of walls, paths, and spaces
  gridConvert(grid2D, fp);
  masterGrid->grid2D = grid2D;

  int NR = sizeof(grid2D)/sizeof(grid2D[0]);                  // number of rows in grid 
  int NC = strlen(grid2D[0]);                                 // number of columns in grid

  stand(seed);                                                // not sure what this means but seed is an optional parameter for server
  // set the number of piles in the map
  int numPiles = (int)(rand() % (minGoldPiles - maxGoldPiles + 1)) + minGoldPiles; 

  piles_t** goldPiles;
  // create pile structures bt setting random locations and random amounts for gold
  for (int i = 0; i < numPiles; i++) {
    position_t* goldPosition= mem_malloc(position_t);
    goldPosition->x = 0;
    goldPosition->y = 0;
    // find random position that is in an empty room spot
    while (!((grid2d[y][x] == EMPTY)) {
      goldPosition->x = ; // need to figure out how to do this
      goldPosition->y = ;
    }
  }

}

