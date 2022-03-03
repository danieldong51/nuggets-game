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
#include "grid.h"
#include "spectator.h"
#include "support/message.h"
#include "support/log.h"



/* ***************************************** */
/* Global types */
struct game {
  int goldRemaining;              // amount of gold left in game 
  int numPlayers; 
  player_t* players[26];
  grid_t* masterGrid; 
  spectator_t* spectator; 

} game; 

/* ***************************************** */
/* Global variables */


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

/* ******************** main() ************************** */
/* calls parseArgs, initializeGame, acceptMessages, and gameOver before exiting*/ 
int main(const int argc, char* argv[])
{
  FILE* mapFile; 

  // validate command-line arguments 
  if ( parseArgs(argc, argv, &mapFile) ) {

    // call initialize game 
    initializeGame(mapFile); 

    // initialize the message module 
    int port = message_init(NULL);                 // will provide file pointer(fp), passed through to log_init()

    // print the port number on which we wait 
    fprintf("Port Number: %d\n", port);

    // call acceptMessages() 

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
  // initialize information about the game 
  game.numPlayers = 0; 

  // initalize master grid 
  game.masterGrid = grid_new();

  // function to conver the map.txt file into a char** 2D array, and save it in this masterGrid struct 
  gridMake(game.masterGrid, mapFile);

  // function to initialize game grid by dropping [GoldTotal] amount of nuggets 
  gridDropGold(game.masterGrid, GoldMinNumPiles, GoldMaxNumPiles, GoldTotal);

}



