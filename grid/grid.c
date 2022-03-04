#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "bag.h"
#include "file.h"
#include "mem.h"
#include <math.h>

/**************** file-local global variables ****************/
static int ROCK = ' ';
static int EMPTY = '.';
static int PASSAGE = '#';
static int HORIZONTAL = '-';
static int VERITAL = '|';
static int CORNER = '+';
static int MAXPLAYERS = 26;
static int MAXGOLD = 50;

/**************** local types ****************/
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

/**************** global types ****************/
typedef struct grid {
  char** grid2D;                    // 2d string array, each slot represents one row
  pile_t** goldPiles; 
  playerAndPosition_t** playerPositions;
  int nrows;
  int ncols; 
} grid_t;

static char** newGrid2D(int nrows, int ncols);
static void gridConvert(char** grid, FILE* fp, int nrows, int ncols);
void updateGrid(grid_t* playerGrid, grid_t* masterGrid, char playerLetter);
void gridPrint(grid_t* map, char playerLetter);
int gridValidMove(grid_t* map, position_t* coordinate);
void gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int seed);
grid_t* gridNewPlayer(grid_t* map);

/**************** local functions ****************/
/* not visible outside this module */
bool isVisible(position_t* playerPos, position_t* squarePos, grid_t* masterGrid);
bool isEmpty(grid_t* masterGrid, int col, int row);
int increment(int cur, int goal);
bool isInteger(float num);
position_t* position_new(int x, int y);
void clearPlayerArray(grid_t* grid);
void clearPileArray(grid_t* grid);
char gridGetChar(char** grid, position_t* position);
void gridMark(char** grid, position_t* position, char mark);
grid_t* grid_new();
int getNumRows(grid_t* masterGrid);
int getNumColumns(grid_t* masterGrid);
char** getGrid2D(grid_t* masterGrid);

/**************** newGrid2D ****************/
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


static void
gridConvert(char** grid, FILE* fp, int nrows, int ncols)
{
  const int size = ncols+2;  // include room for \n\0
  char line[size];           // a line of input
  int y = 0;
<<<<<<< HEAD
=======
  printf("ROWS: %d\n", nrows);
>>>>>>> dev

  // read each line and copy it to the board
  while ( fgets(line, size, fp) != NULL && y < nrows) {
    int len = strlen(line);
    if (line[len-1] == '\n') {
      // normal line
      len--; // don't copy the newline
    } else {
      // overly wide line
      len = ncols;
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
/* See grid.h for details */
void 
updateGrid(grid_t* playerGrid, grid_t* masterGrid, char playerLetter)
{
  int playerIndex = playerLetter - 'a';
  position_t* playerPos = masterGrid->playerPositions[playerIndex]->playerPosition;

  // initialize toVisit bag and visited grid
  bag_t* toVisit = bag_new();
  char** visited = newGrid2D(getNumRows(masterGrid), getNumColumns(masterGrid));

  // clear players and piles of gold in playerGrid
  clearPlayerArray(playerGrid);
  clearPileArray(playerGrid);

  // add current squares to toVisit bag and mark as visited 
  position_t* curPosition = masterGrid->playerPositions[playerIndex]->playerPosition;
  bag_insert(toVisit, curPosition);

  gridMark(visited, curPosition, '.');
  
  // while toVisit is not empty
  position_t* position;
  while ((position = bag_extract(toVisit)) != NULL) {

    // if the position is visible
    if (isVisible(playerPos, position, masterGrid)) {

      // mark square as visible
      gridMark(visited, curPosition, '.');

      // mark player grid
      gridMark(playerGrid->grid2D, position, gridGetChar(masterGrid->grid2D, position));

      // loop over adjacent squares
      bag_t* adjacentSquares = bag_new();

      int x = position->x;
      int y = position->y;

      position_t* positionLeft = position_new(x - 1, y);
      position_t* positionRight = position_new(x + 1, y);
      position_t* positionUp = position_new(x, y + 1);
      position_t* positionDown = position_new(x, y - 1);

      bag_insert(adjacentSquares, positionLeft);
      bag_insert(adjacentSquares, positionRight);
      bag_insert(adjacentSquares, positionUp);
      bag_insert(adjacentSquares, positionDown);

      position_t* positionAdjacent;

      while ((positionAdjacent = bag_extract(adjacentSquares)) != NULL) {

        // if square not visited
        if (gridGetChar(visited, positionAdjacent) != '.') {

          // mark square as visitied
          gridMark(visited, positionAdjacent, '.');

          // add to toVisit
          bag_insert(toVisit, positionAdjacent);
        }
      }
    }
  }
}

void 
clearPlayerArray(grid_t* grid)
{
  for (int i = 0; i < MAXPLAYERS; i++) {
    grid->playerPositions[i] = NULL;
  }
}

void 
clearPileArray(grid_t* grid)
{
  for (int i = 0; i < MAXGOLD; i++) {
    grid->goldPiles[i] = NULL;
  }
}

char 
gridGetChar(char** grid, position_t* position)
{
  return grid[position->y][position->x];
}

void 
gridMark(char** grid, position_t* position, char mark) 
{
  grid[position->y][position->x] = mark;
}

/**************** isVisible ****************/
/* Helper function for updateGrid */
bool 
isVisible(position_t* playerPos, position_t* squarePos, grid_t* masterGrid)
{
  int pcol = playerPos->x;
  int col = squarePos->x;
  int prow = playerPos->y;
  int row = squarePos->y;

  float slope = (row - prow) / (col - pcol);
  float rowSlope = (col - pcol) / (row - prow);

  // looping over rows
  int curRow;
  while ((curRow = increment(curRow, row)) != row) {
    float curCol = pcol + (1 / rowSlope) * (curRow - prow);

    // checks if intersects at only one place
    if (isInteger(curCol)) {

      // if intersects at only one place,
      // checking if that spot is a wall
      if (!isEmpty(masterGrid, (int) curCol, curRow)) {
        return false;
      }

    } else {

      // else intersects at in between place
      // checking both spots to see if spot is empty
      if (!isEmpty(masterGrid, (int) curCol, curRow) && 
          !isEmpty(masterGrid, (int) curCol + 1 , curRow)) {
        return false;
      }
    }
  }

  // looping over columns
  int curCol;
  while ((curCol = increment(curCol, col)) != col) {
    float curRow = prow + (1 / slope) * (curCol - pcol);

    // checks if intersects at only one place
    if (isInteger(curRow)) {

      // if intersects at only one place,
      // checking if that spot is a wall
      if (!isEmpty(masterGrid, curCol, (int) curRow)) {
        return false;
      }

    } else {

      // else intersects at in between place
      // checking both spots to see if spot is empty
      if (!isEmpty(masterGrid, curCol, (int) curRow) && 
          !isEmpty(masterGrid, curCol, (int) curRow + 1)) {
        return false;
      }
    }
  }
  return true;
}

/**************** isEmpty ****************/
/* Helper function for isVisible */
bool 
isEmpty(grid_t* masterGrid, int col, int row)
{
  position_t* intersectPos = position_new(col, row);
  char space = gridGetChar(masterGrid->grid2D, intersectPos);
  free(intersectPos);
  if (space != EMPTY) {
    return false;
  } else {
    return true;
  }
}

int 
increment(int cur, int goal) 
{
  if (cur > goal) {
    return --cur;
  } else {
    return ++cur;
  }
}

bool 
isInteger(float num)
{
  int intNum = (int) num;
  if (num - intNum == 0) {
    return true;
  } else {
    return false;
  }
}

position_t* position_new(int x, int y)
{
  position_t* position = malloc(sizeof(position_t));
  position->x = x;
  position->y = y;
  return position;
}


/**************** gridPrint ****************/
void gridPrint(grid_t* map, char playerLetter)
{
  int players = sizeof(map->playerPositions)/sizeof(map->playerPositions[0]);                   // determine number of players that are to be printed
  position_t* currentPosition = map->playerPositions[playerLetter - 'a']->playerPosition;

  // set the players in the grid array
  for (int i = 0; i < players; i++) {
    position_t* tempPosition = mem_malloc(sizeof(position_t));
    tempPosition = map->playerPositions[i]->playerPosition;
    // print position of the current player
    if ( (tempPosition->x == currentPosition->x) && (tempPosition->y == currentPosition->y)) {    
      map->grid2D[currentPosition->x][currentPosition->y] = '@';
    }
    // print position of the other players 
    else {
      map->grid2D[tempPosition->x][tempPosition->y] = map->playerPositions[i]->name;
    }
    mem_free(tempPosition);
  }

  // set the gold in the grid array
  int numPiles = sizeof(map->goldPiles)/sizeof(map->goldPiles[0]);                  
  for ( int i = 0; i< numPiles; i++ ) {
    position_t* tempPosition = mem_malloc(sizeof(position_t));
    tempPosition = map->goldPiles[i]->location;
    map->grid2D[tempPosition->x][tempPosition->y] = '*';
  }


  // print out each row
  for ( int i = 0; i < map->nrows; i ++ ) {
    fprintf(stdout, "%s", map->grid2D[i]);
  }
}


/**************** gridValidMove ****************/
/* gives an (x,y) position and checks to see if a player can move into that position in the map */
int 
gridValidMove(grid_t* map, position_t* coordinate)
{
  int xCord = coordinate->x;
  int yCord = coordinate->y;

  // if coordinate is an empty room space or passage
  if ( (map->grid2D[yCord][xCord] == EMPTY) || ((map->grid2D[yCord][xCord]) == PASSAGE) ||  ((map->grid2D[yCord][xCord]) == PASSAGE)) {
    return 0;
  }

  // if the coordinate is a pile of gold
  else if ((map->grid2D[yCord][xCord] == '*')) {
    int numPiles = sizeof(map->goldPiles)/sizeof(map->goldPiles[0]);  
    for (int i = 0; i < numPiles; i++) {
      // loop through and find the pile structure with the same position
      if (((map->goldPiles[i]->location->x) == xCord) && ((map->goldPiles[i]->location->y) == yCord)) {
        int goldAmount = map->goldPiles[i]->amount;
        return goldAmount;
      }
    }
  }
  // not a valid space to move into
  return 0;
}


/**************** grid_new ****************/
// initializes a new empty grid--mallocs memory
grid_t* 
grid_new()
{
  grid_t* map = mem_malloc(sizeof(grid_t));
  return map;
}

/**************** gridMakeMaster ****************/
/* fill up char** array and piles_t** array */
void 
gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int seed)
{
  FILE *fp = fopen(fileName, "r");

  // set goldPiles for grid
  int NR = file_numLines(fp);                                 // number of rows in grid 
  char* tempColumn = file_readLine(fp);
  int NC = strlen(tempColumn) + 1;                             // number of columns in grid

  masterGrid->nrows = NR;
  masterGrid->ncols = NC;

    // set 2d char map for grid
  char** grid2D;                                              // map of walls, paths, and spaces
  grid2D = newGrid2D(NR, NC);
  gridConvert(grid2D, fp, NR, NC);
  masterGrid->grid2D = grid2D;

  srand(seed);                                                // not sure what this means but seed is an optional parameter for server
  // set the number of piles in the map
  int numPiles = (int)(rand() % (minGoldPiles - maxGoldPiles + 1)) + minGoldPiles; 
  int currentGoldAmount;

  
  pile_t* goldPiles[numPiles]; // check this syntax
  // create pile structures bt setting random locations and random amounts for gold
  for (int i = 0; i < numPiles; i++) {
    position_t* goldPosition= mem_malloc(sizeof(position_t));
    pile_t* goldPile = mem_malloc(sizeof(pile_t));
    goldPosition->x = 0;
    goldPosition->y = 0;
    // find random position that is in an empty room spot
    while (!((grid2D[goldPosition->y][goldPosition->x ] == EMPTY))) {
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
  fclose(fp);
  // NEED TO THINK ABOUT FREES
}


/**************** gridNewPlayer ****************/
// returns the new player grid, which will start off as completely empty
// creates a new playerAndPosition struct representing new player in the masterGrid (map is the masterGrid)
grid_t* 
gridNewPlayer(grid_t* map)
{
  position_t* playerPosition= mem_malloc(sizeof(position_t));

  playerPosition->x = 0;
  playerPosition->y = 0;
  while ( !(map->grid2D[playerPosition->y][playerPosition->x] == EMPTY) ){
    playerPosition->x = (rand() % map-> nrows) + 1; 
    playerPosition->y = (rand() % map-> ncols) + 1;
  }

  // if this is the first player being intialized
  if (map->playerPositions == NULL) {
    playerAndPosition_t* players[26];
    map->playerPositions = players;
  }

  int i = 0;
  while( !(map->playerPositions[i] == NULL) ) {
    i++;
  }
  map->playerPositions[i]->name = i+'a'; // set char name
  map->playerPositions[i]->playerPosition = playerPosition;

  // Initialize new playerGrid
  grid_t* playerGrid = grid_new(); 
  
  // malloc space for gold piles and players
  playerGrid->playerPositions = malloc(MAXPLAYERS * sizeof(playerAndPosition_t*));
  playerGrid->goldPiles = malloc(MAXGOLD * sizeof(pile_t*));

  return playerGrid;
}

/**************** getNumRows ****************/
int getNumRows(grid_t* masterGrid) {
  return masterGrid->nrows;
}

/**************** getNumColumns ****************/
int 
getNumColumns(grid_t* masterGrid) 
{
  return masterGrid->ncols;
}

/**************** getGrid2D ****************/
char** 
getGrid2D(grid_t* masterGrid) 
{
  return masterGrid->grid2D;
}