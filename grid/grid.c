#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "bag.h"
#include "file.h"
#include "mem.h"
#include "player.h"
#include <math.h>

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
  int nrows;
  int ncols; 
} grid_t;


static int ROCK = ' ';
static int EMPTY = '.';
static int PASSAGE = '#';
static int HORIZONTAL = '-';
static int VERITAL = '|';
static int CORNER = '+';


/**************** newGrid2D ****************/
static char**
newGrid2D(int nrows, int ncols)
{
<<<<<<< HEAD:grid/grid.c

  // allocate a 2-dimensional array of NROWS x NCOLS
  char** grid = calloc(NROWS, sizeof(char*));
  char* contents = calloc(NROWS * NCOLS, sizeof(char));
=======
  // allocate a 2-dimensional array of nrows x ncols
  char** grid = calloc(nrows, sizeof(char*));
  char* contents = calloc(nrows * ncols, sizeof(char));
>>>>>>> 0bae35c5a11b6d1fe827e898ffc0745c77b9fc55:grid.c
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
<<<<<<< HEAD:grid/grid.c
gridConvert(char** grid, FILE* fp, int NROWS, int NCOLS)
=======
gridConvert(char** grid, FILE* fp, int nrows, int ncols)
>>>>>>> 0bae35c5a11b6d1fe827e898ffc0745c77b9fc55:grid.c
{
  const int size = ncols+2;  // include room for \n\0
  char line[size];           // a line of input
  int y = 0;
  printf("ROWS: %d\n", NROWS);

<<<<<<< HEAD:grid/grid.c
  while ( fgets(line, size, fp) != NULL && y <= NROWS) {
=======
  // read each line and copy it to the board
  while ( fgets(line, size, fp) != NULL && y < nrows) {
>>>>>>> 0bae35c5a11b6d1fe827e898ffc0745c77b9fc55:grid.c
    int len = strlen(line);
    printf("line: %s\n", line);
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
    
    strncpy(grid[y], line, len);
    printf("Y: %d\n", y);
    y++;

  }

  if (!feof(fp)) {
    fprintf(stderr, "board too big for screen; truncated to %d lines\r\n", y);
  }
}


/**************** updateGrid ****************/
void updateGrid(player_t* player, grid_t* masterGrid)
{
  int playerIndex = player_getLetter(player) - 'a';
  position_t* playerPos = masterGrid->playerPositions[playerIndex];
  grid_t* playerGrid = player_getGrid(player);

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

bool isVisible(position_t* playerPos, position_t* squarePos, grid_t* masterGrid)
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
}

bool isEmpty(grid_t* masterGrid, int col, int row)
{
  position_t* intersectPos = position_new(col, row);
  char space = gridGetChar(masterGrid, intersectPos);
  free(intersectPos);
  if (space != EMPTY) {
    return false;
  } else {
    return true;
  }
}

int increment(int cur, int goal) 
{
  if (cur > goal) {
    return --cur;
  } else {
    return ++cur;
  }
}

bool isInteger(float num)
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
void gridPrint(grid_t* map, position_t* currentPosition)
{
  int players = sizeof(map->playerPositions)/sizeof(map->playerPositions[0]);                   // determine number of players that are to be printed
  
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
int gridValidMove(grid_t* map, char letter, char move)
{
  int xCord = 0;  // need to change, set to 0 for compiling
  int yCord = 0;

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
  return -1;
}


/**************** grid_new ****************/
// initializes a new empty grid--mallocs memory
grid_t* grid_new(){
  grid_t* map = mem_malloc(sizeof(grid_t));
  return map;
}

/**************** gridMakeMaster ****************/
/* fill up char** array and piles_t** array */
void gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int seed) {
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
  printf("before grid convert\n");
  gridConvert(grid2D, fp, NR, NC);
  printf("grid converted\n");
  masterGrid->grid2D = grid2D;
  printf("1\n");
  srand(seed);                                                // not sure what this means but seed is an optional parameter for server
  // set the number of piles in the map
  int numPiles = (int)(rand() % (minGoldPiles - maxGoldPiles + 1)) + minGoldPiles; 
  int currentGoldAmount;
  printf("2\n");
  
  pile_t* goldPiles[numPiles]; // check this syntax
  // create pile structures bt setting random locations and random amounts for gold
  for (int i = 0; i < numPiles; i++) {
    position_t* goldPosition= mem_malloc(sizeof(position_t));
    pile_t* goldPile = mem_malloc(sizeof(pile_t));
    goldPosition->x = (rand() % NC) + 1;
    goldPosition->y = (rand() % NR) + 1;
    printf("3\n");
    // find random position that is in an empty room spot
    while (!((grid2D[goldPosition->y][goldPosition->x ] == EMPTY))) {
      // set random position for gold
      printf("4\n");
      goldPosition->x = (rand() % NC) + 1; 
      goldPosition->y = (rand() % NR) + 1;
    }
    printf("5\n");
    goldPile->location = goldPosition;
    goldPile->amount = rand();
    currentGoldAmount += goldPile->amount;
    goldPiles[i] = goldPile;
    printf("6\n");
  }
  printf("7\n");
  
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
grid_t* gridNewPlayer(grid_t* map)
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

  grid_t* playerGrid = grid_new();
  return playerGrid;
}

/**************** getNumRows ****************/
int getNumRows(grid_t* masterGrid) {
  return masterGrid->nrows;
}

/**************** getNumColumns ****************/
int getNumColumns(grid_t* masterGrid) {
  return masterGrid->ncols;
}

/**************** getGrid2D ****************/
char** getGrid2D(grid_t* masterGrid) {
  return masterGrid->grid2D;
}

