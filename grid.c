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
void updateGrid(grid_t* playerGrid, grid_t* serverGrid)
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
  
  // set 2d char map for grid
  char** grid2D;                                              // map of walls, paths, and spaces
  gridConvert(grid2D, fp);
  masterGrid->grid2D = grid2D;

  // set goldPiles for grid
  int NR = sizeof(grid2D)/sizeof(grid2D[0]);                  // number of rows in grid 
  int NC = strlen(grid2D[0]);                                 // number of columns in grid

  srand(seed);                                                // not sure what this means but seed is an optional parameter for server
  // set the number of piles in the map
  int numPiles = (int)(rand() % (minGoldPiles - maxGoldPiles + 1)) + minGoldPiles; 
  int currentGoldAmount;

  
  piles_t*[numPiles] goldPiles; // check this syntax
  // create pile structures bt setting random locations and random amounts for gold
  for (int i = 0; i < numPiles; i++) {
    position_t* goldPosition= mem_malloc(position_t);
    pile_t* goldPile = mem_malloc(pile_t);
    goldPosition->x = 0;
    goldPosition->y = 0;
    // find random position that is in an empty room spot
    while (!((grid2d[goldPosition->y][goldPosition->x ] == EMPTY)) {
      // set random position for gold
      goldPosition->x = (rand() % NR) + 1; 
      goldPosition->y = (rand() % NC) + 1;
    }
    goldPile->location = goldPosition;
    goldPile->amount = rand();
    currentGoldAmount += goldPile->amount;
    goldPiles[i] = goldPile;
  }
  
  // fraction to scale down gold amount in each pile by
  int goldScale = numGold/currentGoldAmount;

  // loop through again and scale down gold pile amounts
  for (int i = 0; i< numPiles; i++) {
    int oversizedAmount = goldPiles[i]->amount;
    goldPiles[i]->amount = round(oversizedAmount * goldScale);
  }
  masterGrid->goldPiles=goldPiles;
  // NEED TO THINK ABOUT FREES
}


/**************** gridNewPlayer ****************/
// returns the new player grid, which will start off as completely empty
// creates a new playerAndPosition struct representing new player in the masterGrid (map is the masterGrid)
grid_t* gridNewPlayer(grid_t* map)
{
  int NR = sizeof(map->grid2D)/sizeof(map->grid2D[0]);                  // number of rows in grid 
  int NC = strlen(map->grid2D[0]);                                 // number of columns in grid
  position_t* playerPosition= mem_malloc(position_t);

  playerPosition->x = 0;
  playerPosition->y = 0;
  while ( !(map->grid2D[playerPosition->y][playerPosition->x] == EMPTY) ){
    playerPosition->x = (rand() % NR) + 1; 
    playerPosition->y = (rand() % NC) + 1;
  }

  // if this is the first player being intialized
  if (map->playerPositions == NULL) {
    playerAndPosition_t*[26] players;
    map->playerPositions = players;
  }

  int i = 0;
  while( !(map->playerPositions[i] == NULL) ) {
    i++;
  }
  map->playerPositions[i]->name = i+'a'; // set char name
  map->playerPositions[i]->playerPosition = playerPosition;

  playerGrid = gridInit();
  return playerGrid;
}


