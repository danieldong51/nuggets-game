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
#include "../libcs50/file.h"
#include "../player/player.h"
#include "../grid/grid.h"
#include "../spectator/spectator.h"
#include "../support/message.h"
#include "../support/log.h"

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
  int numPlayers;                 // amount of players that have joined so far 
  player_t** players; 
  grid_t* masterGrid; 
  spectator_t* spectator; 
  int seed;
} game; 



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
static bool parseArgs(const int argc, char* argv[]);
static void initializeGame(char* mapPathname);
static bool handleMessage(void* arg, const addr_t from, const char* message);
void gameOver();

void handlePlayMessage(const addr_t from, const char* message);
void handleSpectateMessage(const addr_t from, const char* message);
void handleKeyMessage(const addr_t otherp, const char* message);

player_t* findPlayer(const addr_t address);
void deleteAllPlayers();

void updateAllGrids();
void sendDisplayToAll();
void sendGoldToAll(int moveResult, player_t* currPlayer);

void sendQuitMessage(const addr_t otherp, char* explanation);
void sendOkMessage(const addr_t otherp, char letter);
void sendGridMessage(const addr_t otherp);
void sendErrorMessage(const addr_t otherp, char* explanation);
void sendGoldMessage(int n, int r, int p, const addr_t otherp);
void sendDisplayMessage(player_t* player, const addr_t otherp);
void sendSpecDisplayMessage(const addr_t otherp);

/* ******************** main() ************************** */
/* calls parseArgs, initializeGame, acceptMessages, and gameOver before exiting*/ 
int main(const int argc, char* argv[])
{

  // validate command-line arguments 
  if ( parseArgs(argc, argv) ) {

    // save pathname 
    char* mapPathname = argv[1];

    // call initialize game 
    initializeGame(mapPathname); 

    // initialize the message module  
    int port = message_init(stderr);                // will eventually pass a log file pointer into here 

    // print the port number on which we wait 
    printf("waiting on port %d for contact....\n", port);

    addr_t other = message_noAddr(); // no correspondent yet 

    // call message_loop() until timeout or error 
    message_loop(&other, timeout, NULL, NULL, handleMessage);
    
    gameOver();

    // shut down message module 
    message_done(); 

  }
  else {
    fprintf(stderr, "Invalid number of arguments\n");
  }
  
}

/* ******************** parseArgs() ************************** */
/* verify that mapfile can be opened and call srand() */ 
static bool parseArgs(const int argc, char* argv[])
{
  if (argc >= 2) {

    // verify that map file can be opened for reading 
    FILE* mapFile = fopen(argv[1],"r");
    if ( mapFile == NULL ) {
      fprintf(stderr, "Unable to open map file\n");
      return false; 
    }
    fclose(mapFile);

    if (argc == 3) {
      // if a seed is provided, verify that it is a positive integer, and then set in game struct 
      int seed = atoi(argv[2]);
        
      if (seed >= 0) {
        game.seed = seed; 
      }
      else {
        fprintf(stderr, "Failed to initialize seed");
        return false; 
      }
    }

    else{
      game.seed = (int) getpid();
    }
    srand(game.seed);
    return true; 
  }
  return false; 
}

/* ******************** initializeGame() ************************** */
/* verify that mapfile can be opened and call srand() */ 
static void initializeGame(char* mapPathname)
{
  // initialize information about the game 
  game.numPlayers = 0; 

  // initialize player list 
  player_t* playersList[MaxPlayers];
  game.players = playersList;

  // intialize each player struct
  player_t* p;
  for (int i = 0; i < MaxPlayers; i++) {
    p = player_new();
    game.players[i] = p;
  }

  // initalize master grid 
  game.masterGrid = grid_new();

  // call rand 
  int randNum = rand();

  // function tto initialize game 
  gridMakeMaster(game.masterGrid, mapPathname, GoldTotal, GoldMinNumPiles, GoldMaxNumPiles, randNum);

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
  // KEY
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

void handlePlayMessage(const addr_t from, const char* message)
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
        *end = '_';
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
      sendQuitMessage(from, "Sorry - you must provide player's name");
      return;
    }

    // check to see if we already have this player 
    if (findPlayer(from) != NULL) {
      sendErrorMessage(from, "Sorry - you cannot rejoin the same game.");
      return;
    }

    game.numPlayers++;
    player_t* player = game.players[game.numPlayers];

    // set this player's status to "true" 
    player_setName(player, name);
    player_setLetter(player, letter);
    player_changeStatus(player, true);
    player_setAddress(player, from);
    
    /// set random position for player and add it to list of positions, also create empty grid for player with grid_new()
    player_setGrid(player, gridNewPlayer(game.masterGrid));

    // send ok message 
    sendOkMessage(from, letter);

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
    sendQuitMessage(from, "Game is full: no more players can join.");
  }
}

void handleSpectateMessage(const addr_t from, const char* message)
{
  // see if we already have a spectator 
  if (game.spectator != NULL) {
    // if we do, look at its address 
    addr_t specAddress = spectator_getAddress(game.spectator);

    if (message_eqAddr(specAddress, from) == false) {
      // if it is not the same address, create a new spectator, replace our spectator, send message to old spectator
      sendQuitMessage(specAddress, "You have been replaced by a new spectator");
      
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
  sendSpecDisplayMessage(from);

}

void handleKeyMessage(const addr_t otherp, const char* message)
{
  bool foundPlayer = false; 
  bool foundSpectator = false; 

  // is this our spectator? 
  if (message_eqAddr(spectator_getAddress(game.spectator), otherp)) {
    foundSpectator = true; 
  }

  // loop through the list of players 
  player_t* currPlayer;

  if ((currPlayer = findPlayer(otherp)) != NULL) {
    foundPlayer = true; 
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
        sendQuitMessage(otherp, "Thanks for playing!");

        // change the player's isTalking status to false 
        player_changeStatus(currPlayer, false);

        
      case 'h': case 'l': case 'j': case 'k': case 'y': case 'u': case 'b': case 'n':
        moveResult = gridValidMove(game.masterGrid, letter, key);
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
        sendQuitMessage(otherp, "Thanks for watching!");
        game.spectator = NULL;
      default: 
        //  the server shall ignore that keystroke and may send back an ERROR message as described below
        sendErrorMessage(otherp, "Invalid keystroke");
    }

  }
}

/*  check parameters, construct the message, log about it, and send the message */
void sendOkMessage(const addr_t otherp, char letter)
{
  if (letter != ' ') {
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
void sendDisplayMessage(player_t* player, const addr_t otherp) 
{ 
  char* grid1D = gridPrint(player_getGrid(player), player_getLetter(player));

  char response[message_MaxBytes];
  sprintf(response, "DISPLAY\n%s", grid1D);

  message_send(otherp, response);
}

/* check parameters, construct message, and send message */
void sendSpecDisplayMessage(const addr_t otherp)
{
  char* grid1D = gridPrint(game.masterGrid, '.');

  char response[message_MaxBytes];
  sprintf(response, "DISPLAY\n%s", grid1D);

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
void sendQuitMessage(const addr_t otherp, char* explanation)
{
  char quitMessage[message_MaxBytes];

  sprintf(quitMessage, "QUIT %s", explanation);

  message_send(otherp, quitMessage); 

}


void sendDisplayToAll()
{
  // loop through players, send DISPLAY message to each one 
  for (int i = 0; i < MaxPlayers; i++) {
    player_t* thisPlayer = game.players[i];
    if (player_isTalking(thisPlayer)) {

      // update grid for this player 
      updateGrid(player_getGrid(thisPlayer), game.masterGrid, player_getLetter(thisPlayer));
      
      // get address of this player 
      addr_t address = player_getAddress(thisPlayer);
    
      // send display message to player 
      sendDisplayMessage(thisPlayer, address); 
    }
  }
  // send to spectator
  addr_t specAddress = spectator_getAddress(game.spectator);
  
  sendSpecDisplayMessage(specAddress);

}
void sendGoldToAll(int moveResult, player_t* currPlayer) 
{
  for (int i = 0; i< MaxPlayers; i++){
    // check if player is currently talking to server 
    player_t* player = game.players[i];

    // get address of player 
    addr_t address = player_getAddress(player);

    if (player_isTalking(player)) {

      if (player_getLetter(player) == player_getLetter(currPlayer)) {
        sendGoldMessage(moveResult, player_getGold(player), game.goldRemaining, address);
      }
      else {
        sendGoldMessage(0, player_getGold(player), game.goldRemaining, address); 
      }
    }
  }

  addr_t specAddress = spectator_getAddress(game.spectator);
  // send to spectator 
  sendGoldMessage(0, 0, game.goldRemaining, specAddress);

}

void updateAllGrids()
{
  // call updateGrid on EVERY player  
  for (int i = 0; i < MaxPlayers; i++) {
    player_t* player = game.players[i];

    if (player_isTalking(player)) {
      updateGrid(player_getGrid(player), game.masterGrid, player_getLetter(player));
    }
  }
}

player_t* findPlayer(const addr_t address) 
{
  for (int i = 0; i < MaxPlayers ; i++)
  {
    player_t* player = game.players[i];

    if (player_isTalking(player)) {
      addr_t playerAddress = player_getAddress(player);

      if (message_eqAddr(playerAddress, address)) {
        return player;
      }
    }
  }
  return NULL;
}

void gameOver()
{
  // construct and broadcast game over message
  char gameOverMessage[message_MaxBytes];
  sprintf(gameOverMessage, "GAME OVER:\n");

  for (int i = 0; i < MaxPlayers; i++) {
    player_t* p = game.players[i];
    if (player_isTalking(game.players[i])){
      char* info;

      // set info pointer to each player line, incrementing each time 
      for (info = gameOverMessage; *info ; info++) {
        sprintf(info, "%c\t%3d %s\n", player_getLetter(p), player_getGold(p), player_getName(p));
      } 
    }
  }
  // broadcast game over message to all players 
  for (int i = 0; i < MaxPlayers; i++) {
    player_t* player = game.players[i];

    if (player_isTalking(player)) {
      sendQuitMessage(player_getAddress(player), gameOverMessage);
    }
  }
  // call player_delete on players
  deleteAllPlayers();

  // delete master grid
  gridDelete(game.masterGrid);

  // delete spectator object
  if (game.spectator != NULL) {
    spectator_delete(game.spectator);
  }
  
}

void deleteAllPlayers()
{
  for (int i = 0; i < MaxPlayers; i++) {
    player_delete(game.players[i]);
  }
}




