/* 
 * grid.c - a program responsible funnctionality for nuggets game play
 * 
 * Nuggets
 * CS50, Winter 2022
 * Team Tux
 */
 
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
  char name;                      // player's letter
  position_t* playerPosition;     // location of player
} playerAndPosition_t;

typedef struct pile {
  position_t* location;           // location of gold Pile
  int amount;                     // amount of gold at the pile
} pile_t;

/**************** global types ****************/
typedef struct grid {
  char** grid2D;                            // 2d string array, each slot represents one row
  pile_t** goldPiles;                       // list of gold piles
  playerAndPosition_t** playerPositions;    // list of players and their positions
  int nrows;                                // nrows in the map
  int ncols;                                // ncols in the map
} grid_t;

/**************** global functions ****************/
void gridConvert(char** grid, FILE* fp, int nrows, int ncols);                            // convert map file to char** object
void updateGrid(grid_t* playerGrid, grid_t* masterGrid, char playerLetter);               // update visibility display within a grid's char**
char* gridPrint(grid_t* map, char playerLetter);                                          // print out a grid's char** map with the gold piles and other players
int gridValidMove(grid_t* masterGrid, char playerLetter, char moveLetter);                // check if a move is a valid move for a player, and make the move if it is valid
void gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int randInt);  // make the mastergrid for the spectator and server
grid_t* gridNewPlayer(grid_t* masterGrid, char playerLetter);                             // make a new player, update the masterGrid
grid_t* grid_new();                                                                       // allocate memory for a new player
int getNumRows(grid_t* masterGrid);                                                       // get number rows in the map
int getNumColumns(grid_t* masterGrid);                                                    // get number columns in the map
char** getGrid2D(grid_t* masterGrid);                                                     // get the char** component of a grid structure
void gridDelete(grid_t* map, bool isMaster);                                              // delete grid and free its memory
void grid_deletePlayer(grid_t* masterGrid, char playerLetter);                            // delete a player from the master grid so that it is no longer printed in the map

/**************** local functions ****************/
/* not visible outside this module */
static char** newGrid2D(int nrows, int ncols);                                            // create a new char** grid given rows and columns, set as empty
static bool isVisible(position_t* playerPos, position_t* squarePos, grid_t* masterGrid);  // Checks if checkPos is visible from playerPos
static bool isEmpty(grid_t* masterGrid, int col, int row);                                // Checks if spot on grid is an empty space
static int increment(int cur, int goal);                                                  // Helper function for updateGrid, increments or decrements cur, depending on if goal is larger or smaller
static bool isInteger(float num);                                                         // Checks if float num is an intege
static position_t* position_new(int x, int y);                                            // Returns new position_t struct with x and y
static char gridGetChar(char** grid, position_t* position);                               // Returns char from grid at position
static void gridMark(char** grid, position_t* position, char mark);                       // Marks the grid at a given positon given a character
static bool checkSpot(grid_t* masterGrid, grid_t* playerGrid, char** visible, position_t* playerPos, position_t* checkPos); // Checks if given spot is visible
static void goldPilesDelete(pile_t** goldPiles, bool isMaster);                           // deletes and frees a list of goldPile structures
static void playerAndPositionDelete(playerAndPosition_t** playerPositions, bool isMaster);// deletes and frees a list of playerAndPosition structures


/**************** newGrid2D ****************/
/* creates and allocates space for an array of strings given number of rows and number of columns */
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

/**************** gridConvert ****************/
/* See grid.h for details */ 
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

  // initialize empty visible grid
  char** visible = newGrid2D(getNumRows(masterGrid), getNumColumns(masterGrid));
  gridMark(visible, playerPos, '.');

  // while there are still visible squares in the loop
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

  // free visible grid
  free(visible[0]);
  free(visible);
}

/**************** checkSpot ****************/
/* Checks if given spot is visible */
bool
checkSpot(grid_t* masterGrid, grid_t* playerGrid, char** visible, position_t* playerPos, position_t* checkPos)
{
  // check if spot is in map
  if (checkPos->x >= 0 && checkPos->y >= 0 && checkPos->x < masterGrid->ncols && checkPos->y < masterGrid->nrows) {

    // if spot is visible
    if (isVisible(playerPos, checkPos, masterGrid)) {

      // mark player grid
      char gridChar = gridGetChar(masterGrid->grid2D, checkPos);
      gridMark(playerGrid->grid2D, checkPos, gridChar);

      // mark visible grid
      gridMark(visible, checkPos, '.');

      return true;
    }
  }
  return false;
}

/**************** gridGetChar ****************/
/* Returns char from grid at position */
char 
gridGetChar(char** grid, position_t* position)
{
  return grid[position->y][position->x];
}

/**************** gridMark ****************/
/* Marks char at grid at position with mark*/
void 
gridMark(char** grid, position_t* position, char mark) 
{
  grid[position->y][position->x] = mark;
}

/**************** isVisible ****************/
/* Checks if checkPos is visible from playerPos */
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
/* Checks if spot on grid is an empty space */
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

/**************** increment ****************/
/* Helper function for updateGrid, increments or decrements
 * cur, depending on if goal is larger or smaller
 */
int 
increment(int cur, int goal) 
{
  if (cur > goal) {
    return --cur;
  } else {
    return ++cur;
  }
}

/**************** isInteger ****************/
/* Checks if float num is an integer */
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

/**************** position_new ****************/
/* Returns new position_t struct with x and y */
position_t* position_new(int x, int y)
{
  position_t* position = malloc(sizeof(position_t));
  position->x = x;
  position->y = y;
  return position;
}


/**************** gridPrint ****************/
/* Returns char* representation of playerGrid */
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
          char playerLetter = i + 'A';
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

        gridMark(returnGrid, pilePosition, GOLDPILE);
      }
    }
  }

  // Converts from char** to char*
  char* returnString;
  returnString = mem_malloc(sizeof(char)* (nrows*(ncols+1)) + 1) ;

  // Loops over rows in char**
  for ( int i = 0; i < nrows; i++ ){
    for ( int x = 0; x < ncols; x++ ) {
      returnString[((i)* (ncols + 1)) + x] = returnGrid[i][x];
    }
    returnString[(i+1)*(ncols+1) - 1] = '\n';
  }

  // Sets end to NULL character
  returnString[(nrows*(ncols+1)) ] = '\0';
  mem_free(returnGrid[0]);
  mem_free(returnGrid);
  return returnString;
}


/**************** gridValidMove ****************/
/* gives an (x,y) position and makes move if possible, returning gold if any found */
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

  // if coordinate is an empty room space or passage
  if ( gridSpot == EMPTY || gridSpot == PASSAGE ) {
    
    // update player location in masterGrid
    masterGrid->playerPositions[index]->playerPosition->x = xCord;
    masterGrid->playerPositions[index]->playerPosition->y = yCord;

    // check if coordinate is pile of gold
    for (int i = 0; i < MAXGOLD; i++) {

      // return gold amount if gold pile has same position
      pile_t* pile;
      if ((pile = masterGrid->goldPiles[i]) != NULL) {

        int xGold = pile->location->x;
        int yGold = pile->location->y;

        // checks if gold pile has same position
        if (xCord == xGold && yCord == yGold) {
          
          // returns amount of gold
          int amount = pile->amount;
          mem_free(masterGrid->goldPiles[i]->location);
          mem_free(masterGrid->goldPiles[i]);
          masterGrid->goldPiles[i] = NULL;
          return amount;
        }
      }
    }

    // return 0 - valid move
    return 0;
  }

  // return -1 - not a valid space
  return -1;
}

/**************** grid_new ****************/
/* initializes a new empty grid--mallocs memory */
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
/* fill up char** array given a map file and piles_t** array give information about gold. Amount of gold in each pile is randomized.  */
void 
gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, 
               int minGoldPiles, int maxGoldPiles, int randInt)
{
  FILE *fp = fopen(fileName, "r");

  // set goldPiles for grid
  int NR = file_numLines(fp);                               // number of rows in grid 
  char* tempColumn = file_readLine(fp);
  int NC = strlen(tempColumn) + 1;                          // number of columns in grid
  mem_free(tempColumn);
  fclose(fp);

  fp = fopen(fileName, "r");
  masterGrid->nrows = NR;                                   // set rows of grid
  masterGrid->ncols = NC;                                   // set columns of grid
  

  // set 2d char map for grid
  char** grid2D;                                            // map of walls, paths, and spaces
  grid2D = newGrid2D(NR, NC);         
  gridConvert(grid2D, fp, NR, NC);                          // fill up the char** array given the map file
  masterGrid->grid2D = grid2D;                              // set the char** grid2D of grid

  // server calls srand(seed) and that is the only time it srand() is called
  // randomize the number of piles in the map
  int numPiles = (int)(randInt % (maxGoldPiles - minGoldPiles + 1)) + minGoldPiles; 
  double currentGoldAmount = 0;
  
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

    // initially set amount of gold in the pile to be large random value
    goldPile->amount = rand();

    // increment variable that keepps track of total gold amounr
    currentGoldAmount = currentGoldAmount + goldPile->amount;  

    goldPiles[i] = goldPile;
  }

  // fraction to scale down gold amount in each pile by
  float goldScale = ((float)numGold/(float)currentGoldAmount);

  // reset tracker variable
  currentGoldAmount = 0;

  // loop through again and scale down gold pile amounts
  for (int i = 0; i< numPiles; i++) {
    int oversizedAmount = goldPiles[i]->amount;
    goldPiles[i]->amount = floor(oversizedAmount * goldScale);      // always floor
    currentGoldAmount = currentGoldAmount + goldPiles[i]->amount;
  }

  // because we floor the amount of gold whem rounding, the current gold may be less than the amount of gold supposed to be in the amp
  if (currentGoldAmount < numGold){
    // scale up the last pile
    goldPiles[numPiles-1]->amount += (numGold-currentGoldAmount);
  }

  masterGrid->goldPiles=goldPiles;
  fclose(fp);
}


/**************** gridNewPlayer ****************/
/* returns the new player grid, which will start off as completely empty
 * creates a new playerAndPosition struct representing new player in the masterGrid
 */
grid_t* 
gridNewPlayer(grid_t* masterGrid, char playerLetter)
{
  // set random location for player
  int y = 0;
  int x = 0;

  while ( (masterGrid->grid2D[y][x] != EMPTY) ) {
    y = (rand() % masterGrid-> nrows); 
    x = (rand() % masterGrid-> ncols);
  }

  // create new position struct for player
  position_t* playerPosition = position_new(x, y);
  int index = playerLetter - 'a';

  // initialize the playerPositions at index
  playerAndPosition_t* playerAndPosition = malloc(sizeof(playerAndPosition_t));

  playerAndPosition->name = playerLetter;
  playerAndPosition->playerPosition = playerPosition;

  masterGrid->playerPositions[index] = playerAndPosition;

  // Initialize new playerGrid
  grid_t* playerGrid = grid_new(); 

  // initialize playerGrid variables
  playerGrid->grid2D = newGrid2D(masterGrid->nrows, masterGrid->ncols);
  playerGrid->ncols = masterGrid->ncols;
  playerGrid->nrows = masterGrid->nrows;
  
  return playerGrid;
}

/**************** getNumRows ****************/
/* Returns number of rows stored in masterGrid */
int 
getNumRows(grid_t* masterGrid) 
{
  return masterGrid->nrows;
}

/**************** getNumColumns ****************/
/* Returns number of columns stored in masterGrid */
int 
getNumColumns(grid_t* masterGrid) 
{
  return masterGrid->ncols;
}

/**************** getGrid2D ****************/
/* Returns grid2D stored in grid */
char** 
getGrid2D(grid_t* masterGrid) 
{
  return masterGrid->grid2D;
}

/**************** gridDelete ****************/
/* Frees grid and its variables */
void 
gridDelete(grid_t* map, bool isMaster) 
{
  // delete gold and player positions
  goldPilesDelete(map->goldPiles, isMaster); // delete the goldPiles
  playerAndPositionDelete(map->playerPositions, isMaster);

  // free map->grid2D
  mem_free(map->grid2D[0]);
  mem_free(map->grid2D);

  mem_free(map);

}

/**************** goldPilesDelete ****************/
/* Helper function for gridDelete, deletes goldPiles if master */
static void 
goldPilesDelete(pile_t** goldPiles, bool isMaster) 
{
  
  if (goldPiles != NULL) {

    if (isMaster) {

      // frees all gold piles
      for (int i = 0; i < MAXGOLD; i++) {
        if (goldPiles[i] != NULL) {
          mem_free(goldPiles[i]->location);
          mem_free(goldPiles[i]);
        }
      }
    }
    mem_free(goldPiles);
  }
}

/**************** playerAndPositionDelete ****************/
/* Helper function for gridDelete, deletes playerPositions if master */
static void 
playerAndPositionDelete(playerAndPosition_t** playerPositions, bool isMaster)
{
  if (isMaster) {

    // loops over player positions
    for (int i = 0; i < MAXPLAYERS; i++) {

      // if not null, free its contents
      if (playerPositions[i] != NULL) {
        mem_free(playerPositions[i]->playerPosition);
      }
      mem_free(playerPositions[i]);
    }
  }
  mem_free(playerPositions);
}

/**************** grid_deletePlayer ****************/
/* Deletes player position from masterGrid, called when player quits */
void grid_deletePlayer(grid_t* masterGrid, char playerLetter) 
{
  playerAndPosition_t** playerPositions = masterGrid->playerPositions;
  int index = playerLetter - 'a';

  // free player if player position exists
  if (playerPositions[index] != NULL) {
    free(playerPositions[index]->playerPosition);
    free(playerPositions[index]);
    playerPositions[index] = NULL;
  }
}
