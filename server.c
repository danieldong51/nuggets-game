/* 
 * server - a program that implements all game logic for the CS50 'Nuggets' game 
 * the game server maintains all game state, and one or more game clients display the game to a user
 * 
 * Nuggets
 * CS50, Winter 2022
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "libcs50/file.h"
#include "player.h"

/* ***************************************** */
/* Global types */
typedef struct game {
  // NOTE: I actually do not think these constants should be in the game struct, I think we misunderstood that lol 
  int maxNameLength; //= 50; 		  // max number of chars in playerName
  int maxPlayers; //= 26;      		// maximum number of players
  int goldTotal; //= 250;     		// amount of gold in the game
  int goldMinNumPiles; //= 10; 		// minimum number of gold piles
  int goldMaxNumPiles; //= 30; 		// maximum number of gold piles


  // I think this should be what's in the game struct: 
  int goldRemaining;              // amount of gold left in game 
  int numPlayers; 
  player_t* players[26];
  grid_t* masterGrid; 

} game_t; 

/* ***************************************** */
/* Global variables */
static game_t* game; 

/* ***************************************** */
/* Global constants */
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles 

/* *********************************************************************** */
/* Public methods */

/* *********************************************************************** */
/* Local methods */
int main(const int argc, char* argv[]);
static bool parseArgs(const int argc, char* argv[], FILE** mapFile);
static void initializeGame(FILE* mapFile);
static void acceptMessages(int port);
bool handleMessage(void* arg, const addr_t from, const char* message);
void gameOver();
game_t* game_new();

/* ******************** main() ************************** */
/* calls parseArgs, initializeGame, acceptMessages, and gameOver before exiting*/ 
int main(const int argc, char* argv[])
{
  FILE* mapFile; 

  // LIST OF PLAYERS?
  // MASTER GRID?
  // ^ or should those be held in the game struct 

  // validate command-line arguments 
  if ( parseArgs(argc, argv, &mapFile) ) {

    // call initialize game 
    initializeGame(mapFile); 
  }
  
}

/* ******************** parseArgs() ************************** */
/* verify that mapfile can be opened and call srand() */ 
static bool parseArgs(const int argc, char* argv[], FILE** mapFile)
{
  // Start from the commandline of the form above; thus the first argument is the pathname for a map file and 
  // the second argument is an optional seed for the random-number generator; 
  // if provided, the seed must be a positive integer.
  if (argc >= 2) {
    // first argument is pathname for map file 
    char* mapPathname = argv[1];

    // verify that map file can be opened for reading 
    if ( (*mapFile = fopen(*mapPathname,'r')) == NULL ) {
      fprintf(stderr, "Unable to open map file\n");
      return false; 
    }

    if (argc == 3) {
      // if a seed is provided, verify that it is a positive integer 
      int seed = atoi(argv[2]);

      if (seed != NULL && seed >= 0) {
        srand(seed);
      }
      else {
        srand(getpid());
      }
    }
    else {
      srand(getpid());
    }
  }
  else {

  }
  return true; 
}

/* ******************** initializeGame() ************************** */
/* verify that mapfile can be opened and call srand() */ 
static void initializeGame(FILE* mapFile)
{
  // call gridConvert to convert the map file into a 2D array 
  char** masterGrid = gridConvert(mapFile);
  // TODO: create a masterGrid grid STRUCT and initialize this as the 2D array, save the grid in the game struct 

  // initialize the game struct 
  game = game_new(); 

  game->maxNameLength = 50; 		  // max number of chars in playerName
  game->maxPlayers = 26;      		// maximum number of players
  game->goldTotal = 250;     			// amount of gold in the game
  game->goldMinNumPiles = 10; 		// minimum number of gold piles
  game->goldMaxNumPiles = 30; 		// maximum number of gold piles

  game->numPlayers = 0; 
  

  
}

game_t* game_new()
{
  game_t* game = mem_melloc_assert(sizeof(game_t), "Unable to allocate memory for game struct\n");
  if (game != NULL) {
    return game; 
  }
  return NULL; 
}


