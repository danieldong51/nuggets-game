#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/bag.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include <math.h>
#include "grid.h"

/**************** file-local global variables ****************/
static int EMPTY = '.';
static int PASSAGE = '#';
static int GOLDPILE = '*';
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

void gridConvert(char** grid, FILE* fp, int nrows, int ncols);
void updateGrid(grid_t* playerGrid, grid_t* masterGrid, char playerLetter);
char* gridPrint(grid_t* map, char playerLetter);
int gridValidMove(grid_t* masterGrid, char playerLetter, char moveLetter);
void gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int randInt);
grid_t* gridNewPlayer(grid_t* masterGrid, char playerLetter);
grid_t* grid_new();
int getNumRows(grid_t* masterGrid);
int getNumColumns(grid_t* masterGrid);
char** getGrid2D(grid_t* masterGrid);
void gridDelete(grid_t* map, bool isMaster);
void grid_deletePlayer(grid_t* masterGrid, char playerLetter);

/**************** local functions ****************/
/* not visible outside this module */
static char** newGrid2D(int nrows, int ncols);
static bool isVisible(position_t* playerPos, position_t* squarePos, grid_t* masterGrid);
static bool isEmpty(grid_t* masterGrid, int col, int row);
static int increment(int cur, int goal);
static bool isInteger(float num);
static position_t* position_new(int x, int y);
static char gridGetChar(char** grid, position_t* position);
static void gridMark(char** grid, position_t* position, char mark);
static bool checkSpot(grid_t* masterGrid, grid_t* playerGrid, char** visible, position_t* playerPos, position_t* checkPos);
static void goldPilesDelete(pile_t** goldPiles, bool isMaster);
static void playerAndPositionDelete(playerAndPosition_t** playerPositions, bool isMaster);


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


void
gridConvert(char** grid, FILE* fp, int nrows, int ncols)
{
  const int size = ncols+2;  // include room for \n\0
  char line[size];           // a line of input
  int y = 0;

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
    
    strncpy(grid[y], line, len);
    y++;

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
  char** visible = newGrid2D(getNumRows(masterGrid), getNumColumns(masterGrid));

  gridMark(visible, playerPos, '.');

  // while there are visible squares in the loop
  bool hasSeen = true;
  int radius = 0;
  while (hasSeen) {

    hasSeen = false;

    // increment radius
    radius++;

    // go around perimeter of square of radius x radius and check visible
    int playerx = playerPos->x;
    int playery = playerPos->y;
    for (int i = -radius; i < radius; i++) {

      // check top side
      position_t* checkPos = position_new(playerx + i, playery - radius);
      if (checkSpot(masterGrid, playerGrid, visible, playerPos, checkPos)) {
        hasSeen = true;
      }

      // check bottom side
      checkPos->y = playery + radius;
      checkPos->x = playerx - i;
      if (checkSpot(masterGrid, playerGrid, visible, playerPos, checkPos)) {
        hasSeen = true;
      }

      // check left side
      checkPos->y = playery - i;
      checkPos->x = playerx - radius;
      if (checkSpot(masterGrid, playerGrid, visible, playerPos, checkPos)) {
        hasSeen = true;
      }

      // check right side
      checkPos->x = playerx + radius;
      checkPos->y = playery + i;
      if (checkSpot(masterGrid, playerGrid, visible, playerPos, checkPos)) {
        hasSeen = true;
      }

      free(checkPos);
    }
  }

  // loop through player positions and add to player grid if visible
  for (int i = 0; i < MAXPLAYERS; i++) {

    // clear playerPosition entry
    playerGrid->playerPositions[i] = NULL;

    // check if not null
    playerAndPosition_t* otherPlayer;
    if ((otherPlayer = masterGrid->playerPositions[i]) != NULL) {

      // check if player position is visible
      if (gridGetChar(visible, otherPlayer->playerPosition) == '.') {

        // add player to playerGrid's player list
        playerGrid->playerPositions[i] = otherPlayer;
      }
    }
  }

  // loop through gold positions and add to player grid if visible
  for (int i = 0; i < MAXGOLD; i++) {

    // clear pilePosition entry
    playerGrid->goldPiles[i] = NULL;

    // check if not null
    pile_t* goldPile;
    if ((goldPile = masterGrid->goldPiles[i]) != NULL) {

      // check if gold position is visible
      if (gridGetChar(visible, goldPile->location) == '.') {

        // add gold to playerGrid's gold list
        playerGrid->goldPiles[i] = goldPile;
      }
    }
  }

  free(visible[0]);
  free(visible);
}

bool
checkSpot(grid_t* masterGrid, grid_t* playerGrid, char** visible, position_t* playerPos, position_t* checkPos)
{
  if (checkPos->x >= 0 && checkPos->y >= 0 && checkPos->x < masterGrid->ncols && checkPos->y < masterGrid->nrows) {
    if (isVisible(playerPos, checkPos, masterGrid)) {

      // mark player grid
      char gridChar = gridGetChar(masterGrid->grid2D, checkPos);
      gridMark(playerGrid->grid2D, checkPos, gridChar);

      //mark visible grid
      gridMark(visible, checkPos, '.');

      return true;
    }
  }
  return false;
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
isVisible(position_t* playerPos, position_t* checkPos, grid_t* masterGrid)
{
  int pcol = playerPos->x;
  int prow = playerPos->y;
  int col = checkPos->x;
  int row = checkPos->y;


  // looping over rows
  int curRow = prow;
  while (row != prow && (curRow = increment(curRow, row)) != row) {

    float rowSlope = (col - pcol) / (row - prow);
    float curCol = pcol + rowSlope * (curRow - prow);

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
  int curCol = pcol;
  while (col != pcol && (curCol = increment(curCol, col)) != col) {

    float slope = (row - prow) / (col - pcol);
    float curRow = prow + slope * (curCol - pcol);

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
char* gridPrint(grid_t* playerGrid, char playerLetter)
{
  // initialize return grid
  int nrows = getNumRows(playerGrid);
  int ncols = getNumColumns(playerGrid);
  char** returnGrid = newGrid2D(nrows, ncols);

  // fill in returnGrid with walls and spaces
  for (int i = 0; i < nrows; i++) {
    strncpy(returnGrid[i], playerGrid->grid2D[i], ncols-1);
  }

  // printing player positions to returnGrid
  // using the playerPositions list in playerGrid
  playerAndPosition_t** playerPositions = playerGrid->playerPositions;

  if (playerPositions != NULL) {
    
    // loop over players
    for (int i = 0; i < MAXPLAYERS; i++) {

      // if player position exists, add to returnGrid
      if (playerPositions[i] != NULL && playerPositions[i]->playerPosition != NULL) {

        position_t* playerPosition = playerPositions[i]->playerPosition;

        // if player is current player, set char to '@'
        if (i == playerLetter - 'a') {
          gridMark(returnGrid, playerPosition, '@');
        } else {
          char playerLetter = i + 'a';
          // set char of player to its player letter
          gridMark(returnGrid, playerPosition, playerLetter);
        }
      }
    }
  }

  // printing gold positions to returnGrid
  // using the goldPiles list in playerGrid
  pile_t** goldPiles = playerGrid->goldPiles;
  if (goldPiles != NULL) {
    
    // loop over gold piles
    for (int i = 0; i < MAXGOLD; i++) {

      // if gold pile exists, add to returnGrid
      if (goldPiles[i] != NULL) {
        
        position_t* pilePosition = goldPiles[i]->location;

        if (pilePosition == NULL) {
          printf("NULLn\n");
        }

        gridMark(returnGrid, pilePosition, GOLDPILE);
      }
    }
  }

  char* returnString;
  returnString = mem_malloc(sizeof(char)* (nrows*(ncols+1)) + 1) ;
  for ( int i = 0; i < nrows; i++ ){
    for ( int x = 0; x < ncols; x++ ) {
      returnString[((i)* (ncols + 1)) + x] = returnGrid[i][x];
    }
    returnString[(i+1)*(ncols+1) - 1] = '\n';
  }
  returnString[(nrows*(ncols+1)) ] = '\0';
  mem_free(returnGrid[0]);
  mem_free(returnGrid);
  return returnString;
}


/**************** gridValidMove ****************/
/* gives an (x,y) position and checks to see if a player can move into that position in the map */
int 
gridValidMove(grid_t* masterGrid, char playerLetter, char moveLetter)
{
  // find the current location of the player
  int index = playerLetter - 'a';
  int yCord;
  int xCord;

  if (masterGrid->playerPositions[index] != NULL) {
    yCord = masterGrid->playerPositions[index]->playerPosition->y;
    xCord = masterGrid->playerPositions[index]->playerPosition->x;
  }
    
  // change to the location based on moveLetter
  if (moveLetter == 'h') {    
    // left
    xCord--;
  }
  else if (moveLetter == 'l') {
    // right
    xCord++;
  }
  else if (moveLetter == 'j') {
    // down
    yCord++;
  }
  else if (moveLetter == 'k') {
    // up
    yCord--;
  }
  else if (moveLetter == 'y') {
    // diagonally up and left
    xCord--;
    yCord--;
  }
  else if (moveLetter == 'u') {
    // diagonally up and right
    xCord++;
    yCord--;
  }
  else if (moveLetter == 'b') {
    // diagonally down and left
    xCord--;
    yCord++;
  }
  else if (moveLetter == 'n') {
    // diagonally down and right
    xCord++;
    yCord++;
  }

  // checking if out of bounds
  if (xCord < 0 || yCord < 0 || xCord >= masterGrid->ncols || yCord >= masterGrid->nrows) {
    return -1;
  }

  char gridSpot = masterGrid->grid2D[yCord][xCord];
  printf("gridSpot is %c\n", gridSpot);

  // if coordinate is an empty room space or passage
  if ( gridSpot == EMPTY || gridSpot == PASSAGE ) {
    
    printf("valid move\n");

    // free existing position

    // update player location in masterGrid
    masterGrid->playerPositions[index]->playerPosition->x = xCord;
    masterGrid->playerPositions[index]->playerPosition->y = yCord;

    // check if coordinate is pile of gold

    for (int i = 0; i < MAXGOLD; i++) {

      // if gold pile has same position, return gold amount
      pile_t* pile;
      if ((pile = masterGrid->goldPiles[i]) != NULL) {

        int xGold = pile->location->x;
        int yGold = pile->location->y;

        // gold pile has same position
        if (xCord == xGold && yCord == yGold) {
          
          int amount = pile->amount;
          mem_free(masterGrid->goldPiles[i]->location);
          mem_free(masterGrid->goldPiles[i]);
          masterGrid->goldPiles[i] = NULL;
          return amount;
        }
      }
    }

    return 0;
  }

  // not a valid space to move into
  return -1;
}

/**************** grid_new ****************/
// initializes a new empty grid--mallocs memory
grid_t* 
grid_new()
{
  grid_t* grid = mem_malloc(sizeof(grid_t));

  // initializing player Positions
  grid->playerPositions = calloc(MAXPLAYERS, sizeof(playerAndPosition_t*));
  for (int i = 0; i < MAXPLAYERS; i++) {
    grid->playerPositions[i] = NULL;
  } 

  // initializing gold piles
  grid->goldPiles = calloc(MAXGOLD, sizeof(pile_t*));
  for (int i = 0; i < MAXGOLD; i++) {
    grid->goldPiles[i] = NULL;
  }

  return grid;
}

/**************** gridMakeMaster ****************/
/* fill up char** array and piles_t** array */
void 
gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int randInt)
{
  FILE *fp = fopen(fileName, "r");

  // set goldPiles for grid
  int NR = file_numLines(fp);                                 // number of rows in grid 
  char* tempColumn = file_readLine(fp);
  int NC = strlen(tempColumn) + 1;                             // number of columns in grid
  mem_free(tempColumn);
  fclose(fp);
  fp = fopen(fileName, "r");
  masterGrid->nrows = NR;
  masterGrid->ncols = NC;
  

    // set 2d char map for grid
  char** grid2D;                                              // map of walls, paths, and spaces
  grid2D = newGrid2D(NR, NC);
  gridConvert(grid2D, fp, NR, NC);
  masterGrid->grid2D = grid2D;
  // server calls srand(seed) and that is the only time it srand() is called
  // set the number of piles in the map
  int numPiles = (int)(randInt % (maxGoldPiles - minGoldPiles + 1)) + minGoldPiles; 
  double currentGoldAmount = 0;

  printf("numpiles: %d\n", numPiles);
  
  pile_t** goldPiles = masterGrid->goldPiles; 


  // create pile structures by setting random locations and random amounts for gold
  for (int i = 0; i < numPiles; i++) {
    position_t* goldPosition = position_new(0, 0);
    pile_t* goldPile = mem_malloc(sizeof(pile_t));


    // find random position that is in an empty room spot
    while (!((grid2D[goldPosition->y][goldPosition->x] == EMPTY))) {
      // set random position for gold
      goldPosition->x = (rand() % NC); 
      goldPosition->y = (rand() % NR);
    }

    goldPile->location = goldPosition;
    goldPile->amount = rand();


    currentGoldAmount = currentGoldAmount + goldPile->amount;  

    goldPiles[i] = goldPile;
  }

  // fraction to scale down gold amount in each pile by
  float goldScale = ((float)numGold/(float)currentGoldAmount);
  currentGoldAmount = 0;
  // loop through again and scale down gold pile amounts
  for (int i = 0; i< numPiles; i++) {
    int oversizedAmount = goldPiles[i]->amount;

    goldPiles[i]->amount = floor(oversizedAmount * goldScale);

    currentGoldAmount = currentGoldAmount + goldPiles[i]->amount;

  }

  if (currentGoldAmount < numGold){
    goldPiles[numPiles-1]->amount += (numGold-currentGoldAmount);
  }


  masterGrid->goldPiles=goldPiles;
  fclose(fp);

}


/**************** gridNewPlayer ****************/
// returns the new player grid, which will start off as completely empty
// creates a new playerAndPosition struct representing new player in the masterGrid
grid_t* 
gridNewPlayer(grid_t* masterGrid, char playerLetter)
{
  int y = 0;
  int x = 0;

  while ( (masterGrid->grid2D[y][x] != EMPTY) ) {

    y = (rand() % masterGrid-> nrows); 
    x = (rand() % masterGrid-> ncols);
  }

  printf("Player pos row is %d, col is %d\n", y, x);

  position_t* playerPosition = position_new(x, y);

  int index = playerLetter - 'a';

  // initialize the playerPositions at index
  playerAndPosition_t* playerAndPosition = malloc(sizeof(playerAndPosition_t));

  playerAndPosition->name = playerLetter;
  playerAndPosition->playerPosition = playerPosition;

  masterGrid->playerPositions[index] = playerAndPosition;

  // TEST
  if (masterGrid->playerPositions[index] == NULL) {
    printf("THIS IS NULL\n");
  }

  // Initialize new playerGrid
  grid_t* playerGrid = grid_new(); 

  // malloc space for grid2D
  playerGrid->grid2D = newGrid2D(masterGrid->nrows, masterGrid->ncols);

  playerGrid->ncols = masterGrid->ncols;
  playerGrid->nrows = masterGrid->nrows;
  
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

void setPosition(position_t* position, int x, int y)
{
  position->x = x;
  position->y = y;
}

position_t* newPosition(){
  position_t* position = mem_malloc(sizeof(position_t));
  return position;
}

void 
gridDelete(grid_t* map, bool isMaster) 
{
  if (map->playerPositions[0] == NULL && isMaster) {
    printf("THIS IS NULL, isMaster is\n");
  }

  goldPilesDelete(map->goldPiles, isMaster); // delete the goldPiles
  playerAndPositionDelete(map->playerPositions, isMaster);

  mem_free(map->grid2D[0]);
  mem_free(map->grid2D);
  mem_free(map);

}

static void 
goldPilesDelete(pile_t** goldPiles, bool isMaster) 
{
  
  if (goldPiles != NULL) {

    if (isMaster) {
      for (int i = 0; i < MAXGOLD; i++) {
        
        if (goldPiles[i] != NULL) {
          mem_free(goldPiles[i]->location);
          mem_free(goldPiles[i]);
        }
        printf("\n");
      }
    }
    mem_free(goldPiles);
  }
}

static void 
playerAndPositionDelete(playerAndPosition_t** playerPositions, bool isMaster)
{
  if (isMaster) {
    for (int i = 0; i < MAXPLAYERS; i++) {
      printf("int i is %d - \n", i);
      if (playerPositions[i] != NULL) {
        printf("in here deleting once\n");
        mem_free(playerPositions[i]->playerPosition);
      }
      mem_free(playerPositions[i]);
    }
  }
  mem_free(playerPositions);
}

void grid_deletePlayer(grid_t* masterGrid, char playerLetter) 
{
  playerAndPosition_t** playerPositions = masterGrid->playerPositions;
  int index = playerLetter - 'a';

  if (playerPositions[index] != NULL) {
    free(playerPositions[index]->playerPosition);
    free(playerPositions[index]);
    playerPositions[index] = NULL;
  }
}
