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
#include "../player/player.h"
#include "../grid/grid.h"
#include "../spectator/spectator.h"
#include "support/message.h"
#include "support/log.h"

/* ***************************************** */
/* Global constants */
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles 
const float timeout = 90; 
const char SPACE = ' ';

/* ***************************************** */
/* Global types */
struct game {
  int goldRemaining;              // amount of gold left in game 
  int numPlayers; 
  player_t* players[MaxPlayers + 1];
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

void sendOkMessage(const addr_t otherp, char letter);
void sendGridMessage(const addr_t otherp);
void sendGoldMessage(int n, int r, int p, const addr_t otherp);
void sendDisplayMessage(char* grid, const addr_t otherp);

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
      fprintf(stderr, "Failure to initialize message module\n");
      exit(1);
    }

    // print the port number on which we wait 
    printf("waiting on port %d for contact....\n", port);

    addr_t other = message_noAddr(); // no correspondent yet 

    // call message_loop() until timeout or error 
    bool ok = message_loop(&other, timeout, NULL, NULL, handleMessage);
    
    gameover();

    // shut down message module 
    message_done(); 

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
  game.masterGrid = grid_new();

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
  

  char* address = inet_ntoa(from.sin_addr);     // IP address of the sender
  int portNum = ntohs(from.sin_port);           // port number of the sender
  char* message = message;                      // message from sender 


  // PLAY
  if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {

    handlePlayMessage(from, message);

    return false; 
  }
  // SPECTATE 
  else if (strncmp(message, "SPECTATE ", strlen("SPECTATE ")) == 0) {
    
    handleSpectateMessage(from, message);

    return false; 

  }

  else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {

    // call handleKey() function
    handleKeyMessage(from, message);

    return false;   // return false to keep looping 
    
  }

  // end the game if no more gold 
  if (game.goldRemaining == 0) {
    return true; 
  }
  
  return false;

}

void handlePlayMessage(const addr_t from, char* message)
{
  const char* content = message + strlen("PLAY ");        // pointer to message, starting after strlen play? 
    
  if (game.numPlayers < MaxPlayers)
  {
    // create a pointer that starts in middle of message
    char* name = strchr(content, SPACE);  
    // TODO: this is the same as "content" ^ 
      
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
      message_send(from, "QUIT Sorry - you must provide player's name.");
    }

    // create a new player 
    player_t* player = player_new(name, letter, game.masterGrid);

    // set this player's status to "true" 
    player_changeStatus(player, true);      

    if (player == NULL) {
      fprintf(stderr, "Unable to allocate memory for new player\n");
      return false; 
    }

    // iterate numPlayers
    game.players[game.numPlayers] = player; 
    game.numPlayers++; 

    // send ok message 
    sendOkMessage(from, letter);

    // set random position for player and add it to list of positions 
    // initializes 
    gridNewPlayer(game.masterGrid);

    updateAllGrids();

    // server shall then immediately send GRID, GOLD, and DISPLAY messages as described below.
    sendGridMessage(from); 

    // creating a new player, so n and p should be 0 
    sendGoldMessage(0, 0, game.goldRemaining, from);


    // send display message to all clients 
    sendDisplayToAll();

  }
  else 
  {
    // too many players, respond to client with "NO"
    message_send(from, "QUIT Game is full: no more players can join.");
  }
}

void handleSpectateMessage(const addr_t from, char* message)
{
  // see if we already have a spectator 
  if (game.spectator != NULL) {
    // if we do, look at its address 
    addr_t* specAddress = spectator_getAddress(game.spectator);

    if (!message_eqAddress(specAddress, from)) {
      // if it is not the same address, create a new spectator, replace our spectator, send message to old spectator
      message_send(*specAddress, "QUIT You have been replaced by a new spectator.");
      
      // delete old spectator
      spectator_delete(game.spectator); 

      // create new spectator
      spectator_t* newSpectator = spectator_new(game.masterGrid, from); 
      
      // set game spectator as new spectator 
      game.spectator = newSpectator; 
    }
  }

  // server shall then immediately send GRID, GOLD, and DISPLAY messages as described below.
  sendGridMessage(from);

  sendGoldMessage(0, 0, game.goldRemaining, from);

  // send masterGrid to spectator 
  sendDisplayMessage(getGrid2D(game.masterGrid), from);

}

void handleKeyMessage(const addr_t otherp, char* message)
{
  bool foundPlayer = false; 
  bool foundSpectator = false; 

  // is this our spectator? 
  if (message_eqAddress(spectator_getAddress(game.spectator), otherp)) {
    foundSpectator = true; 
  }

  // loop through the list of players 
  player_t* currPlayer;

  for (int i = 0; i < game.numPlayers; i++) {
    currPlayer = game.players[i];
    
    // check if this address if our 'from' address
    if (message_eqAddress(player_getAddress(currPlayer), otherp)) {

      // check if this player is still talking to the server 
      if (player_isTakling(currPlayer)){
        // if it is, break - this is our player
        foundPlayer = true; 
        break; 
      }
      else {
        // if this address exists for a player that has already quit the game, send error messages
        // previous players cannot rejoin the game 
        message_send(otherp, "ERROR players cannot rejoin game");
      }
      
    }
  }
  if (foundPlayer) {
    // get the letter of this player 
    char letter = player_getLetter(currPlayer);

    const char* content = message + strlen("KEY "); // pointer to message, starting after "KEY "

    char key; 

    // parse using sscanf 
    sscanf(content, " %c", &key);
    int moveResult; 

    // switch value of key 
    switch(key) {
      case 'Q':
        message_send(otherp, "QUIT Thanks for playing!");

        // change the player's isTalking status to false 
        player_changeStatus(currPlayer, false);

        
      case 'h': case 'l': case 'j': case 'k': case 'y': case 'u': case 'b': case 'n':
        moveResult = gridValidMove(letter, key);
      default: 
        //  the server shall ignore that keystroke and may send back an ERROR message as described below
        sendErrorMessage(otherp, "Invalid keystroke");
    }

    // based on moveResult value, send messages to all clients 

    // if the players keystroke causes them to move to a new spot, 
    // updateGrid for every player
    // inform all clients of a change in the game grid using a DISPLAY message as described below
    if (moveResult == 0) {
      updateAllGrids();
      sendDisplayToAll();
    }
    else if (moveResult == -1) {
      sendErrorMessage(otherp, "Player cannot make this move");
    }
    else {
      // moveResult = amount of gold this player has just picked up 
      // inform all clients of new gold count by sending a "GOLD" message 
      player_addGold(currPlayer, moveResult);
      game.goldRemaining = game.goldRemaining - moveResult;
      sendGoldToAll(moveResult, currPlayer);
    }
  }

  else if (foundSpectator) {
    const char* content = message + strlen("KEY "); // pointer to message, starting after "KEY "

    char key; 

    // parse using sscanf 
    sscanf(content, " %c", &key);

    // switch value of key  - spectator can only send 'Q' keystroke 
    switch(key) {
      case 'Q':
        // delete spectator  
        spectator_delete(game.spectator); 

        message_send(otherp, "QUIT Thanks for watching!");
      default: 
        //  the server shall ignore that keystroke and may send back an ERROR message as described below
        sendErrorMessage(otherp, "Invalid keystroke");
    }

  }
}

/*  check parameters, construct the message, log about it, and send the message */
void sendOkMessage(const addr_t otherp, char letter)
{
  if (letter != NULL) {
    // send "ok" message
    char okMessage[message_MaxBytes];
    sprintf(okMessage, "OK %c", letter);
    message_send(otherp, okMessage);
  }
}

/*  check parameters, construct the message, log about it, and send the message */
void sendGridMessage(const addr_t otherp)
{
  // GRID nrows ncols
  int numRows = getNumRows(game.masterGrid); 
  int numCols = getNumColumns(game.masterGrid);

  char response[message_MaxBytes];
    
  // construct content of message 
  sprintf(response, "%s %d %d", "GRID ", numRows, numCols);

  message_send(otherp, response);


}

/*  check parameters, construct the message, log about it, and send the message */
void sendGoldMessage(int n, int r, int p, const addr_t otherp)
{
  char response[message_MaxBytes];

  sprintf(response, "GOLD %d %d %d", n, r, p);

  message_send(otherp, response); 

}

/*  check parameters, construct the message, log about it, and send the message */
void sendDisplayMessage(char* grid, const addr_t otherp) 
{
  char response[message_MaxBytes];
  sprintf(response, "DISPLAY\n%s", grid);

  message_send(otherp, response);
}

/*  check parameters, construct the message, log about it, and send the message */
void sendErrorMessage(const addr_t otherp, char* explanation)
{
  // log an error, ignore message, send error message to client 
  char response[message_MaxBytes];
  sprintf(response, "ERROR %s", explanation); 

  // log an error 

  message_send(otherp, response); 
}

/*  check parameters, construct the message, log about it, and send the message */
void sendErrorMessage(const addr_t otherp, char* explanation)
{
  // log an error, ignore message, send error message to client 
  char response[message_MaxBytes];
  sprintf(response, "ERROR %s", explanation); 

  // log an error 

  message_send(otherp, response); 
}

void sendDisplayToAll()
{
  // loop through players, send DISPLAY message to each one 
  for (int i = 0; i < game.numPlayers; i++) {
    player_t* thisPlayer = game.players[i];
    if (player_isTakling(thisPlayer)) {

      // update grid for this player 
      updateGrid(player_getGrid(thisPlayer), game.masterGrid, player_getLetter(thisPlayer));
      
      // get address of this player 
      addr_t* address = player_getAddress(thisPlayer);
    
      // send display message to player 
      sendDisplayMessage(player_getGrid(thisPlayer), *address); 
    }
  }
  // send to spectator
  addr_t* specAddress = spectator_getAddress(game.spectator);
  sendDisplayMessage(spectator_getGrid(game.spectator), *specAddress);

}
void sendGoldToAll(int moveResult, player_t* currPlayer) 
{
  for (int i = 0; i< game.numPlayers; i++){
    // check if player is currently talking to server 
    player_t* player = game.players[i];

    // get address of player 
    addr_t* address = player_getAddress(player);

    if (player_isTakling(player)) {

      if (player_getLetter(player) == player_getLetter(currPlayer)) {
        sendGoldMessage(moveResult, player_getGold(player), game.goldRemaining, *address);
      }
      else {
        sendGoldMessage(0, player_getGold(player), game.goldRemaining, *address); 
      }
    }
  }

  addr_t* specAddress = spectator_getAddress(game.spectator);
  // send to spectator 
  sendGoldMessage(0, 0, game.goldRemaining, *specAddress);

}

void updateAllGrids()
{
  // call updateGrid on EVERY player  
  for (int i = 0; i < game.numPlayers; i++) {
    player_t* player = game.players[i];

    if (player_isTakling(player)) {
      updateGrid(player_getGrid(player), game.masterGrid, player_getLetter(player));
    }
  }
}



