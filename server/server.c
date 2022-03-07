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
static void gameOver();

static void handlePlayMessage(const addr_t from, const char* message);
static void handleSpectateMessage(const addr_t from, const char* message);
static void handleKeyMessage(const addr_t otherp, const char* message);

static player_t* findPlayer(const addr_t address);
static void deleteAllPlayers();
static char* getName(const char* content);
static void handleMoveResult(int moveResult, player_t* currPlayer, addr_t otherp);

static void updateAllGrids();
static void sendDisplayToAll();
static void sendGoldToAll(int moveResult, player_t* currPlayer);

static void sendQuitMessage(const addr_t otherp, char* explanation);
static void sendOkMessage(const addr_t otherp, char letter);
static void sendGridMessage(const addr_t otherp);
static void sendErrorMessage(const addr_t otherp, char* explanation);
static void sendGoldMessage(int n, int r, int p, const addr_t otherp);
static void sendDisplayMessage(player_t* player, const addr_t otherp);
static void sendSpecDisplayMessage(const addr_t otherp);

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
    int port = message_init(stderr);                

    // print the port number on which we wait 
    printf("waiting on port %d for contact....\n", port);

    
    // call message_loop() until error, or game over 
    message_loop(NULL, 0, NULL, NULL, handleMessage);
    
    // call game over function once done looping
    printf("getting out of message_loop, calling gameOver\n");
    gameOver();

    // shut down message module 
    message_done(); 

    exit(0);

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
        fprintf(stderr, "Failed to initialize seed\n");
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
  game.players = calloc(MaxPlayers, sizeof(player_t*));

  // intialize each player struct
  for (int i = 0; i < MaxPlayers; i++) {
    game.players[i] = player_new();
  }

  // initalize master grid 
  game.masterGrid = grid_new();

  // call rand 
  int randNum = rand();

  // function tto initialize game 
  gridMakeMaster(game.masterGrid, mapPathname, GoldTotal, GoldMinNumPiles, GoldMaxNumPiles, randNum);
  game.goldRemaining = GoldTotal;

  // initialize spectator struct with masterGrid, but no address 
  spectator_t* spectator = spectator_new(game.masterGrid, message_noAddr()); 
  game.spectator = spectator; 

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
  bool noPlayersTalking = true; 
  // check parameters 
  if (message == NULL || !message_isAddr(from)) {
     return false;
   }

  // PLAY
  if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
    handlePlayMessage(from, message);

  }
  // SPECTATE 
  else if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0) {
    handleSpectateMessage(from, message);
    
  }
  // KEY
  else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {

    // call handleKey() function
    handleKeyMessage(from, message);


    
  }
  else {
    sendErrorMessage(from, "Unknown command.\n");
  }

  // end the game if no more gold 
  if (game.goldRemaining == 0) {
    return true; 
  }
  for (int i = 0; i < MaxPlayers; i++) {
    if (player_isTalking(game.players[i])) {
      printf("Player %c still talking...\n", player_getLetter(game.players[i]));
      noPlayersTalking = false; 
    }
  }
  // if all players have quit, and there's no spectator, quit 
  if (game.numPlayers > 0 && noPlayersTalking){
    printf("returning true because no players talking...\n");
    return true;
  }

  
  return false;

}

static void handlePlayMessage(const addr_t from, const char* message)
{
  const char* content = message + strlen("PLAY ");        // pointer to message, starting after strlen play? 
    
  if (game.numPlayers < MaxPlayers)
  {
    // get name from rest of message 
    char* name = getName(content); 

    // get the letter of this player from numPlayers
    char letter = game.numPlayers + 'a'; 

    if (sizeof(name) == 0) {
      sendQuitMessage(from, "Sorry - you must provide player's name.\n");
      return;
    }

    // check if address is invalid 
    if (message_isAddr(from) == false){
      return;
    }
    // check to see if we already have this address, or if address is invalid 
    player_t* p; 
    if ( (p = findPlayer(from)) != NULL) {
      printf("Found player trying to play again!\n");
      sendErrorMessage(from, "Sorry - you cannot rejoin the same game.\n");
      return;
    }

    // send ok message 
    sendOkMessage(from, letter);
    
    // set this player's status to "true", set name, letter, and address of player 
    player_setName(game.players[game.numPlayers], name);
    player_setLetter(game.players[game.numPlayers], letter);
    player_changeStatus(game.players[game.numPlayers], true);
    player_setAddress(game.players[game.numPlayers], from);

    /// set random position for player and add it to list of positions, also create empty grid for player with grid_new()
    player_setGrid(game.players[game.numPlayers], gridNewPlayer(game.masterGrid, player_getLetter(game.players[game.numPlayers])));

    // call updateGrid to recalculate visibility for all players, including this one 
    updateAllGrids();

    // server shall then immediately send GRID and GOLD messages to new clients
    sendGridMessage(from); 

    // creating a new player, so n and p should be 0 
    sendGoldMessage(0, 0, game.goldRemaining, from);

    // send display message to all clients 
    sendDisplayToAll();

    // incremement the number of players 
    game.numPlayers++;

  }
  else 
  {
    // too many players, respond to client with "NO"
    sendQuitMessage(from, "Game is full: no more players can join.\n");
  }
}

static void handleSpectateMessage(const addr_t from, const char* message)
{
  printf("Called handleSpectate\n");
  // see if we already have a spectator 
  addr_t gameSpecAdd = spectator_getAddress(game.spectator);

  if (message_isAddr(gameSpecAdd)) {

    // if we do, compare the address to this new address 
    if (message_isAddr(from) == true && message_eqAddr(gameSpecAdd, from) == false) {

      // send message to old spectator telling them they are being replaced 
      sendQuitMessage(gameSpecAdd, "You have been replaced by a new spectator.\n");
    }
  }

  // either way, check if this address is valid and set it as spectator's address 
  // send messages 
  if (message_isAddr(from)){
    // set address of spectator to this new spectator 
    spectator_t* newSpectator = game.spectator;
    spectator_setAddress(newSpectator, from);

    // server shall then immediately send GRID, GOLD, and DISPLAY messages as described below.
    sendGridMessage(from);

    sendGoldMessage(0, 0, game.goldRemaining, from);

    // send masterGrid to spectator 
    sendSpecDisplayMessage(from);
  }
  

}

static void handleKeyMessage(const addr_t otherp, const char* message)
{
  printf("Handling key...\n");
  bool foundPlayer = false; 
  bool foundSpectator = false; 
  // is this our spectator? 
  if (message_eqAddr(spectator_getAddress(game.spectator), otherp)) {
    foundSpectator = true; 
  }

  // loop through the list of players 
  player_t* currPlayer;
  if ((currPlayer = findPlayer(otherp)) != NULL) {
    printf("Found player\n");
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
      case 'Q' : case 'q':
        sendQuitMessage(otherp, "Thanks for playing!\n");

        // change the player's isTalking status to false 
        player_changeStatus(currPlayer, false);
        //grid_deletePlayer(game.masterGrid, player_getLetter(currPlayer));
        updateAllGrids();
        sendDisplayToAll();
        break;

      case 'h': case 'l': case 'j': case 'k': case 'y': case 'u': case 'b': case 'n':
        moveResult = gridValidMove(game.masterGrid, letter, key);
        handleMoveResult(moveResult, currPlayer, otherp);
        break;

      case 'H': case 'L': case 'J': case 'K': case 'Y': case 'U': case 'B': case 'N':
        key = tolower(key);
        moveResult = gridValidMove(game.masterGrid, letter, key);
        handleMoveResult(moveResult, currPlayer, otherp);
        break;

      default: 
        //  the server shall ignore that keystroke and may send back an ERROR message as described below
        sendErrorMessage(otherp, "Invalid keystroke\n");
    }
  }

  else if (foundSpectator) {
    const char* content = message + strlen("KEY "); // pointer to message, starting after "KEY "

    char key; 

    // parse using sscanf 
    sscanf(content, " %c", &key);

    // switch value of key  - spectator can only send 'Q' keystroke 
    switch(key) {
      case 'Q': case 'q':
        // set spectator's address to no address
        spectator_setAddress(game.spectator, message_noAddr());
        sendQuitMessage(otherp, "Thanks for watching!\n");
        break;
      default: 
        //  the server shall ignore that keystroke and may send back an ERROR message as described below
        sendErrorMessage(otherp, "Invalid keystroke\n");
    }
  }
}

static void handleMoveResult(int moveResult, player_t* currPlayer, addr_t otherp) 
{
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
    updateAllGrids(); 
    sendGoldToAll(moveResult, currPlayer);
    sendDisplayToAll();
  }
}
/*  check parameters, construct the message, log about it, and send the message */
static void sendOkMessage(const addr_t otherp, char letter)
{
  if (message_isAddr(otherp) && letter != ' ') {
    // send "ok" message
    char okMessage[message_MaxBytes];
    sprintf(okMessage, "OK %c", letter);
    message_send(otherp, okMessage);
  }
}

/*  check parameters, construct the message, log about it, and send the message */
static void sendGridMessage(const addr_t otherp)
{
  if (message_isAddr(otherp)) {
    // GRID nrows ncols
    int numRows = getNumRows(game.masterGrid); 
    int numCols = getNumColumns(game.masterGrid);

    char response[message_MaxBytes];
      
    // construct content of message 
    sprintf(response, "%s %d %d", "GRID ", numRows, numCols);

    message_send(otherp, response);
  }

}

/*  check parameters, construct the message, log about it, and send the message */
static void sendGoldMessage(int n, int r, int p, const addr_t otherp)
{
  if (message_isAddr(otherp) && n >= 0 && p >= 0 && r >= 0) {
    char response[message_MaxBytes];

    sprintf(response, "GOLD %d %d %d", n, r, p);

    message_send(otherp, response); 
  }

}

/*  check parameters, construct the message, log about it, and send the message */
static void sendDisplayMessage(player_t* player, const addr_t otherp) 
{ 
  if (player != NULL && message_isAddr(otherp)){
    char* grid1D = gridPrint(player_getGrid(player), player_getLetter(player));

    char response[message_MaxBytes];
    sprintf(response, "DISPLAY\n%s", grid1D);

    printf("Server Display Grid: \n");
    printf("%s",grid1D);

    message_send(otherp, response);
  }
  
}

/* check parameters, construct message, and send message */
static void sendSpecDisplayMessage(const addr_t otherp)
{
  if (message_isAddr(otherp)){
    char* grid1D = gridPrint(game.masterGrid, '.');

    char response[message_MaxBytes];
    sprintf(response, "DISPLAY\n%s", grid1D);

    message_send(otherp, response);
  }

}

/*  check parameters, construct the message, log about it, and send the message */
static void sendErrorMessage(const addr_t otherp, char* explanation)
{
  if (message_isAddr(otherp) && explanation != NULL){
    char response[message_MaxBytes];
    sprintf(response, "ERROR %s", explanation); 

    message_send(otherp, response); 
  }
  
}

/*  check parameters, construct the message, log about it, and send the message */
static void sendQuitMessage(const addr_t otherp, char* explanation)
{
  if (message_isAddr(otherp) && explanation != NULL) {
    char quitMessage[message_MaxBytes];

    sprintf(quitMessage, "QUIT %s", explanation);

    message_send(otherp, quitMessage); 
  }
  
}


static void sendDisplayToAll()
{
  // loop through players, send DISPLAY message to each one 
  for (int i = 0; i < MaxPlayers; i++) {
    player_t* thisPlayer = game.players[i];
    if (player_isTalking(thisPlayer) && message_isAddr(player_getAddress(thisPlayer))) {

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
  
  if (message_isAddr(specAddress)){
    sendSpecDisplayMessage(specAddress);
  }
  

}
static void sendGoldToAll(int moveResult, player_t* currPlayer) 
{
  for (int i = 0; i< MaxPlayers; i++){
    // check if player is currently talking to server 
    player_t* player = game.players[i];

    // get address of player 
    addr_t address = player_getAddress(player);

    if (player_isTalking(player) && message_isAddr(player_getAddress(player))) {

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
  if (message_isAddr(specAddress)){
    sendGoldMessage(0, 0, game.goldRemaining, specAddress);
  }
}

static void updateAllGrids()
{
  // call updateGrid on EVERY player  
  for (int i = 0; i < MaxPlayers; i++) {
    player_t* player = game.players[i];

    if (player_isTalking(player)) {
      updateGrid(player_getGrid(player), game.masterGrid, player_getLetter(player));
    }
  }
  // don't need to update grid for spectator
}

player_t* findPlayer(const addr_t address) 
{
  if (message_isAddr(address)) {
    for (int i = 0; i < MaxPlayers ; i++)
    {
      if (message_eqAddr(player_getAddress(game.players[i]), address)) {
        return game.players[i];
      }
    }
  }
  return NULL;
}

static void gameOver()
{
  // construct and broadcast game over message
  char gameOverMessage[message_MaxBytes];
  printf("gameOver called..\n");
  sprintf(gameOverMessage, "GAME OVER:\n");

  for (int i = 0; i < MaxPlayers; i++) {
    player_t* p = game.players[i];
    printf("first loop\n");
    if (strcmp(player_getName(p),"")!=0){
      printf("print player info for %s\n", player_getName(p));
      // set info pointer to each player line, incrementing each time 
      char* info;
      for (info = gameOverMessage ; *info ; info++);
      sprintf(info, "%c\t%3d %s\n", player_getLetter(p), player_getGold(p), player_getName(p));
      } 
  }
  

  // broadcast game over message to all players who are still talking to server 
  for (int i = 0; i < MaxPlayers; i++) {
    player_t* player = game.players[i];
    printf("second loop\n");
    if (player_isTalking(player) && message_isAddr(player_getAddress(player))) {
      sendQuitMessage(player_getAddress(player), gameOverMessage);
    }
  }
  // send spectator game over message 
  addr_t specAddress = spectator_getAddress(game.spectator);
  // send to spectator 
  if (message_isAddr(specAddress)){
    printf("send to spectator\n");
    sendQuitMessage(specAddress, gameOverMessage);
  }
  // call player_delete on players
  deleteAllPlayers();
  free(game.players);

  // delete master grid
  gridDelete(game.masterGrid, true);

  // delete spectator object
  if (game.spectator != NULL) {
    spectator_delete(game.spectator);
  }
  
}

static void deleteAllPlayers()
{
  for (int i = 0; i < MaxPlayers; i++) {
    player_delete(game.players[i]);
  }
}

static char* getName(const char* content)
{
  // create a pointer that starts in middle of message
  char* name = mem_malloc_assert(sizeof(char)*(MaxNameLength + 1), "Unable to allocate memory for player name.\n");

  // loop through content to get name 
  int i; 
  for (i = 0; i < strlen(content) && i < MaxNameLength; i++){
    if ( (!isgraph(content[i])) && !(isblank(content[i])) ) {
      name[i]='_';
    }
    else {
      name[i] = content[i];
    }
  }
  name[i] = '\0';

  return name; 
  
}




