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
static int ROCK = ' ';
static int EMPTY = '.';
static int PASSAGE = '#';
static int HORIZONTAL = '-';
static int VERTICAL = '|';
static int CORNER = '+';
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
void gridDelete(grid_t* map);
void goldPilesDelete(pile_t** goldPiles);
void playerAndPositionDelete(playerAndPosition_t** playerPositions);

/**************** local functions ****************/
/* not visible outside this module */
static char** newGrid2D(int nrows, int ncols);
static bool isVisible(position_t* playerPos, position_t* squarePos, grid_t* masterGrid);
static bool isEmpty(grid_t* masterGrid, int col, int row);
static int increment(int cur, int goal);
static bool isInteger(float num);
static position_t* position_new(int x, int y);
static void clearPlayerArray(grid_t* grid);
static void clearPileArray(grid_t* grid);
static char gridGetChar(char** grid, position_t* position);
static void gridMark(char** grid, position_t* position, char mark);
bool checkSpot(grid_t* masterGrid, grid_t* playerGrid, char** visible, position_t* playerPos, position_t* checkPos);

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
  // printf("ROWS: %d\n", nrows);

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

  // while there are visible squares in the loop
  bool hasSeen = true;
  int radius = 0;
  while (hasSeen) {

    // increment radius
    radius++;

    // go around perimeter of square of radius x radius and check visible
    int playerx = playerPos->x;
    int playery = playerPos->y;
    for (int i = -radius; i < radius; i++) {

      printf("in the for loop, with i as %d\n", i);

      // check top side
      position_t* checkPos = position_new(playerx + i, playery - radius);
      if (checkSpot(masterGrid, playerGrid, visible, playerPos, checkPos)) {
        hasSeen = true;
      }

      printf("checked top\n");

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
    }
  }

  // loop through player positions and add to player grid if visible
  for (int i = 0; i < MAXPLAYERS; i++) {

    // clear playerPosition entry
    playerGrid->playerPositions[i] = NULL;

    // check if not null
    position_t* otherPlayerPos;
    if ((otherPlayerPos = masterGrid->playerPositions[i]->playerPosition) != NULL) {

      // check if player position is visible
      if (gridGetChar(visible, otherPlayerPos) == '.') {
        
        // add player to playerGrid's player list
        playerGrid->playerPositions[i]->playerPosition = otherPlayerPos;
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
}

bool
checkSpot(grid_t* masterGrid, grid_t* playerGrid, char** visible, position_t* playerPos, position_t* checkPos)
{
  if (isVisible(playerPos, checkPos, masterGrid)) {

    // mark player grid
    char gridChar = gridGetChar(masterGrid->grid2D, checkPos);
    gridMark(playerGrid->grid2D, checkPos, gridChar);

    //mark visible grid
    gridMark(visible, checkPos, '.');

    return true;
  }
  return false;
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
isVisible(position_t* playerPos, position_t* checkPos, grid_t* masterGrid)
{
  int pcol = playerPos->x;
  int prow = playerPos->y;
  int col = checkPos->x;
  int row = checkPos->y;

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
char* gridPrint(grid_t* playerGrid, char playerLetter)
{
  // printf("%ld\n", sizeof(playerGrid->playerPositions));
  // printf("%ld\n", sizeof(playerGrid->playerPositions[0]));
  // int players = sizeof(playerGrid->playerPositions)/sizeof(playerGrid->playerPositions[0]);                   // determine number of players that are to be printed
  // position_t* currentPosition = playerGrid->playerPositions[playerLetter - 'a']->playerPosition;


  // initialize return grid
  int nrows = getNumRows(playerGrid);
  int ncols = getNumColumns(playerGrid);
  char** returnGrid = newGrid2D(nrows, ncols);

  // fill in returnGrid with walls and spaces
  for (int i = 0; i < nrows; i++) {
    strcpy(returnGrid[i], playerGrid->grid2D[i]);
  }

  // printing player positions to returnGrid
  // using the playerPositions list in playerGrid
  playerAndPosition_t** playerPositions = playerGrid->playerPositions;

  printf("filled in returnGrid\n");

  if (playerPositions != NULL) {
    
    // loop over players
    for (int i = 0; i < MAXPLAYERS; i++) {

      // if player position exists, add to returnGrid
      if (playerPositions[i] != NULL && playerPositions[i]->playerPosition != NULL) {

        printf("playerPositions added\n");

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

  printf("made it past players\n");

  // printing gold positions to returnGrid
  // using the goldPiles list in playerGrid
  pile_t** goldPiles = playerGrid->goldPiles;
  if (goldPiles != NULL) {
    
    // loop over gold piles
    for (int i = 0; i < MAXGOLD; i++) {

      // if gold pile exists, add to returnGrid
      if (goldPiles[i] != NULL) {
        
        printf("inside...\n");

        position_t* pilePosition = goldPiles[i]->location;

        if (pilePosition == NULL) {
          printf("NULLn\n");
        }

        printf("before here\n");

        printf("x: %d, y: %d\n", pilePosition->x, pilePosition->y);

        printf("here\n");

        gridMark(returnGrid, pilePosition, GOLDPILE);

        printf("gridMark...\n");
      }
    }
  }

  printf("made it past gold\n");

  char* returnString;
  returnString = mem_malloc(sizeof(char)* (nrows*(ncols+1)) + 1) ;
  for ( int i = 0; i < nrows; i++ ){
    for ( int x = 0; x < ncols; x++ ) {
      returnString[((i)* (ncols + 1)) + x] = returnGrid[i][x];
    }
    returnString[(i+1)*(ncols+1) - 1] = '\n';
    // if (i != 0 ){
    //       returnString[(i)*(ncols)] = '\n';
    // }
    //returnString[(i+1)*(ncols)] = '\n';
    // if (i != 0) {
    //   returnString[i*(ncols+1)] = '\n';
    // }
  }
  returnString[(nrows*(ncols+1)) + 1 ] = '\0';

  return returnString;
}


/**************** gridValidMove ****************/
/* gives an (x,y) position and checks to see if a player can move into that position in the map */
int 
gridValidMove(grid_t* masterGrid, char playerLetter, char moveLetter)
{
  // find the current location of the player
  int i;
  position_t* coordinate = position_new(0,0);
  for (i =0; i<26; i++) {
    if (masterGrid->playerPositions[i]->name == playerLetter) {
      coordinate = masterGrid->playerPositions[i]->playerPosition;
    }
  }
  
  // change to the location based on moveLetter
  if (moveLetter == 'h') {    
    // left
    coordinate->x --;
  }
  else if (moveLetter == 'l') {
    // right
    coordinate->x ++;
  }
  else if (moveLetter == 'j') {
    // down
    coordinate->y --;
  }
  else if (moveLetter == 'k') {
    // up
    coordinate->y ++;
  }
  else if (moveLetter == 'y') {
    // diagonally up and left
    coordinate->x --;
    coordinate->y ++;
  }
  else if (moveLetter == 'u') {
    // diagonally up and right
    coordinate->x ++;
    coordinate->y ++;
  }
  else if (moveLetter == 'b') {
    // diagonally down and left
    coordinate->x --;
    coordinate->y --;
  }
  else if (moveLetter == 'n') {
    // diagonally down and right
    coordinate->x ++;
    coordinate->y --;
  }


  int xCord = coordinate->x;
  int yCord = coordinate->y;

  // if coordinate is an empty room space or passage
  if ( (masterGrid->grid2D[yCord][xCord] == EMPTY) || ((masterGrid->grid2D[yCord][xCord]) == PASSAGE) ||  ((masterGrid->grid2D[yCord][xCord]) == PASSAGE)) {
    masterGrid->playerPositions[i]->playerPosition = coordinate;
    return 0;
  }

  // if the coordinate is a pile of gold
  else if ((masterGrid->grid2D[yCord][xCord] == '*')) {
    int numPiles = sizeof(masterGrid->goldPiles)/sizeof(masterGrid->goldPiles[0]);  
    for (int x = 0; x < numPiles; x++) {
      // loop through and find the pile structure with the same position
      if (((masterGrid->goldPiles[i]->location->x) == xCord) && ((masterGrid->goldPiles[i]->location->y) == yCord)) {
        masterGrid->playerPositions[i]->playerPosition = coordinate;
        int goldAmount = masterGrid->goldPiles[i]->amount;
        return goldAmount;
      }
    }
  }
  // not a valid space to move into
  return -1;
}

//tester
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
  int currentGoldAmount;

  printf("numpiles: %d\n", numPiles);
  
  pile_t** goldPiles = masterGrid->goldPiles; 

  // create pile structures by setting random locations and random amounts for gold
  for (int i = 0; i < numPiles; i++) {

    position_t* goldPosition= position_new(0, 0);
    pile_t* goldPile = mem_malloc(sizeof(pile_t));

    // find random position that is in an empty room spot
    while (!((grid2D[goldPosition->y][goldPosition->x] == EMPTY))) {
      // set random position for gold
      goldPosition->x = (rand() % NC); 
      goldPosition->y = (rand() % NR);
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

  position_t* playerPosition = position_new(x, y);

  // // if this is the first player being intialized
  // if (masterGrid->playerPositions == NULL) {
  //   playerAndPosition_t* players[26];
  //   masterGrid->playerPositions = players;
  // }

  // get the index of the next available spot
  // int i = 0;
  // while( !(masterGrid->playerPositions[i] == NULL) ) {
  //   i++;
  // }

  int index = playerLetter - 'a';

  // initialize the playerPositions at index
  playerAndPosition_t* playerAndPosition = malloc(sizeof(playerAndPosition_t));
  playerAndPosition->name = playerLetter;
  playerAndPosition->playerPosition = playerPosition;

  masterGrid->playerPositions[index] = playerAndPosition;

  // Initialize new playerGrid
  grid_t* playerGrid = grid_new(); 

  // malloc space for grid2D
  playerGrid->grid2D = newGrid2D(masterGrid->nrows, masterGrid->ncols);
  
  // // malloc space for gold piles and players - DONE IN GRID_NEW()
  // playerGrid->playerPositions = malloc(MAXPLAYERS * sizeof(playerAndPosition_t*));
  // playerGrid->goldPiles = malloc(MAXGOLD * sizeof(pile_t*));

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

void gridDelete(grid_t* map) {
  goldPilesDelete(map->goldPiles); // delete the goldPiles
  playerAndPositionDelete(map->playerPositions);
  mem_free(map);
}

void goldPilesDelete(pile_t** goldPiles) 
{
  int numPiles = sizeof(goldPiles) / sizeof(goldPiles[0]);
  for (int i = 0; i < numPiles; i++) {
    mem_free(goldPiles[i]);
  }
}

void playerAndPositionDelete(playerAndPosition_t** playerPositions)
{
  int numPlayers = sizeof(playerPositions) / sizeof(playerPositions[0]);
  for (int i = 0; i < numPlayers; i++) {
    mem_free(playerPositions[i]);
  }
}