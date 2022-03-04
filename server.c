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
  int seed;
} game; 

/****************** types *********************/
/* A type representing an Internet address, suitable for use in message_send().
 * Although not technically opaque (because struct sockaddr_in is well
 * documented and not made opaque by the include file <arpa/inet.h> that
 * defines it), users of this module should treat addr_t as an opaque type.
 * Module users can declare variables of type addr_t, and initialize them
 * to the value returned by message_noAddr, or initialize them in a call to
 * message_setAddr, or receive them as a parameter in one of the handler
 * functions.  Addresses can be passed by value to and from functions, but they
 * cannot be compared directly for equality; to compare two addresses,
 * use message_eqAddr.
 */
//typedef struct sockaddr_in addr_t;

typedef struct position {
  int x;
  int y;
} position_t;

/* ***************************************** */
/* Global variables */


/* ***************************************** */
/* Global constants */
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles 
const float timeout = 90; 
const char SPACE = ' ';
/* *********************************************************************** */
/* Public methods */

/* *********************************************************************** */
/* Local methods */
int main(const int argc, char* argv[]);
static bool parseArgs(const int argc, char* argv[], FILE** mapFile);
static void initializeGame(FILE* mapFile);
static void acceptMessages();
static bool handleMessage(void* arg, const addr_t from, const char* message);
void gameOver();
bool handleTimeout(void* arg); 

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
    int port = message_init(NULL);                // will eventually pass a log file pointer into here 

    if (port == 0) {
      fprintf("Failure to initialize message module\n");
      exit(1);
    }

    // print the port number on which we wait 
    printf("waiting on port %d for contact....\n", port);

    addr_t other = message_noAddr(); // no correspondent yet 

    // call message_loop() until timeout or error 
    bool ok = message_loop(&other, timeout, handleTimeout, NULL, handleMessage);

    // shut down message module 
    message_done(); 
    
    gameover();

  }
  
}

/* ******************** parseArgs() ************************** */
/* verify that mapfile can be opened and call srand() */ 
static bool parseArgs(const int argc, char* argv[], FILE** mapFile)
{
  if (argc >= 2) {
    // first argument is pathname for map file 
    char* mapPathname = argv[1];

    // verify that map file can be opened for reading 
    if ( (*mapFile = fopen(*mapPathname,'r')) == NULL ) {
      fprintf(stderr, "Unable to open map file\n");
      return false; 
    }

    if (argc == 3) {
      // if a seed is provided, verify that it is a positive integer, and then set in game struct 
      int seed = atoi(argv[2]);

      if (seed != NULL && seed >= 0) {
        game.seed = seed; 
      } 
    }
    return true; 
  }
  return false; 
}

/* ******************** initializeGame() ************************** */
/* verify that mapfile can be opened and call srand() */ 
static void initializeGame(FILE* mapFile)
{
  // initialize information about the game 
  game.numPlayers = 0; 

  // initalize master grid 
  game.masterGrid = gridInit();

  // function tto initialize game 
  gridMakeMaster(game.masterGrid, mapFile, GoldTotal, GoldMinNumPiles, GoldMaxNumPiles, game.seed);

}

/* Datagram received; print it.
 * We use 'arg' to carry an addr_t referring to the 'other' correspondent.
 * If 'other' is a valid client, leave it unchanged.
 * If 'other' is not a valid client, update *from to this sender.
 * Return true if the message loop should exit, otherwise false.
 * e.g., return true if fatal error.
 */
static bool handleMessage(void* arg, const addr_t from, const char* message)
{
  addr_t* otherp = (addr_t* )arg;
  if (otherp == NULL) { // defensive
    fprintf(stderr, "handleMessage called with arg=NULL\n");
    return true;
  }

  // this sender becomes our correspondent, henceforth
  *otherp = from;

  char* address = inet_ntoa(from.sin_addr);     // IP address of the sender
  int portNum = ntohs(from.sin_port);           // port number of the sender
  char* message = message;                      // message from sender 

  if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
    const char* content = message + strlen("PLAY ");
    
    if (game.numPlayers < MaxPlayers)
    {
      // create a pointer that starts in middle of message
      char* name = strchr(content, SPACE);  
      
      // create end pointer and begin it after the space (at first letter of name)
      char* end = ++name;

      // move pointer along, keep track of name length 
      int nameLen = 0; 
      while (*end != '\0' && nameLen < MaxNameLength) {
        //  replace with an underscore _ any character for which both isgraph() and isblank() are false
        if (!isgraph(*end) || !(isblank(*end))) {
          *end = "_";
        }
        // slide pointer until it is null or we have reached max name length
        end++; 
      }
      // once we reach end of name or max name length, squash with pointer 
      if (*end != '\0')
      {
        *end = '\0';
      }
      

      // get the letter of this player from numPlayers
      char letter = game.numPlayers + 'a'; 

      if (sizeof(name) == 0) {
        message_send(*otherp, "QUIT Sorry - you must provide player's name.");
      }

      // create a new player 
      player_t* player = player_new(name, letter, game.masterGrid);

      if (player == NULL) {
        fprintf(stderr, "Unable to allocate memory for new player\n");
        return false; 
      }

      // send "ok" message
      char* okMessage = mem_malloc_assert(strlen("OK") + 2, "Unable to allocate memory for message\n");
      sprintf(okMessage, "OK %c", letter);
      message_send(*otherp, okMessage);

      // call updateGrid on player 
      updateGrid(player, game.masterGrid);

      // server shall then immediately send GRID, GOLD, and DISPLAY messages as described below.
      sendGridMessage(otherp); 

      // creating a new player, so n and p should be 0 
      sendGoldMessage(0, 0, game.goldRemaining, otherp);

      sendDisplayMessage(player_getGrid(player),otherp);

    }
    else 
    {
      // too many players, respond to client with "NO"
      message_send(*otherp, "QUIT Game is full: no more players can join.");
    }
  } 
  else if (strncmp(message, "SPECTATE ", strlen("SPECTATE ")) == 0) {

    // initialize game.spectator or replace spectator if it already exists 
    spectator_t* spectator = spectator_new(game.masterGrid);
    game.spectator = spectator; 

    // server shall then immediately send GRID, GOLD, and DISPLAY messages as described below.
    sendGridMessage(otherp);

    sendGoldMessage(0, 0, game.goldRemaining, otherp);

    // send masterGrid to spectator 
    sendDisplayMessage(getGrid2D(game.masterGrid), otherp);

  }
  


  
  fflush(stdout);
  return false;



}

void sendGridMessage(const addr_t otherp)
{
  // GRID nrows ncols
  int numRows = getNumRows(game.masterGrid); 
  int numCols = getNumColumns(game.masterGrid);

  const char* response = mem_malloc_assert(strlen("GRID ") + sizeof(int) + sizeof(int) + 1, "Unable to allocate memory for message content\n");
    
  // construct content of message 
  sprintf(response, "%s %d %d", "GRID ", numRows, numCols);

  message_send(*otherp, response);


}

/* arg is a player object, if given */ 
void sendGoldMessage(int n, int r, int p, const addr_t otherp)
{
  const char* response = mem_malloc_assert(strlen("GOLD ") + sizeof(int)*3 + 1, "Unable to allocate memory for message\n");
  sprintf(response, "GOLD %d %d %d", n, r, p);

  message_send(*otherp, response); 

}

void sendDisplayMessage(char* grid, const addr_t otherp) {
  const char* response = mem_malloc_assert(strlen("GOLD ") + strlen(grid) + 1, "Unable to allocate memory for message\n");
  sprintf(response, "DISPLAY\n%s", grid);

  message_send(*otherp, response);
}


