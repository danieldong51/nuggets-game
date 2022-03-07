/* 
 * server - a program that implements all game logic for the CS50 'Nuggets' game 
 * 
 * 
 * Nuggets
 * CS50, Winter 2022
 * Team Tux
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
static const char SPACE = ' ';
static const char PERIOD = '.'; 

/* ***************************************** */
/* Global variables */
struct game {
  int goldRemaining;              // amount of gold left in game 
  int numPlayers;                 // amount of players that have joined so far 
  player_t** players;             // list of player structs 
  grid_t* masterGrid;             // master grid struct
  spectator_t* spectator;         // spectator struct
  int seed;                       // seed for rand()
} game; 

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
static void handleMoveResult(int moveResult, player_t* currPlayer, addr_t otherp);

static player_t* findPlayer(const addr_t address);
static void deleteAllPlayers();
static char* getName(const char* content);

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
/* calls parseArgs, initializeGame, message_loop, and gameOver before exiting */ 
int main(const int argc, char* argv[])
{

  // validate command-line arguments 
  if (parseArgs(argc, argv)) {
 
    char* mapPathname = argv[1];        // save pathname

    initializeGame(mapPathname); 
  
    // initialize the message module and announce port 
    int port = message_init(stderr);         
    printf("waiting on port %d for contact....\n", port);

    // call message_loop() until error, or game over 
    message_loop(NULL, 0, NULL, NULL, handleMessage);
    
    gameOver();
  
    message_done();                    // shut down message module

    exit(0);

  }
  
}

/* ******************** parseArgs() ************************** */
/* verify that mapfile can be opened, initialize seed, and call srand() */ 
static bool parseArgs(const int argc, char* argv[])
{
  char* progName = argv[0];

  // check number of args
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Invalid number of arguments\n");
    fprintf(stderr, "usage: %s map.txt [seed]\n", progName);
    exit(1);
  }

  // verify that map file can be opened for reading 
  FILE* mapFile = fopen(argv[1],"r");
  if ( mapFile == NULL ) {
    fprintf(stderr, "Unable to open map file of the form: %s\n", argv[1]);
    return false; 
  }
  fclose(mapFile);

  if (argc == 3) {           

    // if seed provided, scan as integer 
    if (sscanf(argv[2], "%d", &game.seed) != 1) {
      fprintf(stderr, "Failed to initialize seed\n");                 // scan unsuccessfull
      return false; 
    }
    if (game.seed < 0) {
      fprintf(stderr, "Invalid seed: must be a positive integer\n");  // invalid seed 
      return false; 

    }
  }
  else {
    game.seed = (int) getpid();                                       // if no seed provided, use getpid()
  }

  srand(game.seed);
  
  return true;
  
}

/* ******************** initializeGame() ************************** */
/* A function to initialize the variables in the game struct  */
/*
* Caller provides: 
*   a pathname for a map.txt file
* We do:  
*   initialize the number of players in the game to zero 
*   create memory for the list of players, and then initialize each player struct
*   initialize masterGrid with grid_new(), and call gridMakeMaster
*   initialize booleans for quitting game
*   initialize spectator 
*   initialize goldRemaining to GoldTotal
* We return: 
*   nothing
*/ 
static void initializeGame(char* mapPathname)
{
  // initialize information about the game 
  game.numPlayers = 0; 

  
  game.players = calloc(MaxPlayers, sizeof(player_t*));   // initialize player list 
  for (int i = 0; i < MaxPlayers; i++) {                  // intialize each player struct 
    game.players[i] = player_new();
  }

  // call rand 
  int randNum = rand();

  game.masterGrid = grid_new();                          // initalize master grid 

  // call function to initialize masterGrid 
  gridMakeMaster(game.masterGrid, mapPathname, GoldTotal, GoldMinNumPiles, GoldMaxNumPiles, randNum);
  game.goldRemaining = GoldTotal;

  // initialize spectator struct with masterGrid, but no address 
  spectator_t* spectator = spectator_new(game.masterGrid, message_noAddr()); 
  game.spectator = spectator; 

}

/* ******************** handleMessage() ************************** */
/* Handler function for message_loop, parses messages */
/*
* Caller provides: 
*   'arg' to carry an addr_t referring to the 'other' correspondent, NULL in our case 
*   add_t from, address of the sender
*   const char* message, a message from the server
* We do:  
*   receive messages from the client
*   call message handler functions to update the game grid and update clients 
* We return: 
*   false to keep looping, for valid or unknown commands, and if message is null 
*   true to terminate message_loop(), if the game is over  
*/ 
static bool handleMessage(void* arg, const addr_t from, const char* message)
{
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
    handleKeyMessage(from, message);
  }
  else {
    sendErrorMessage(from, "Unknown command.\n");     // malformatted message; ignore 
  }

  // end the game if no more gold 
  if (game.goldRemaining == 0) {                      
    return true; 
  }

  // check if any more players are talking
  bool noPlayersTalking = true; 
  for (int i = 0; i < MaxPlayers; i++) {
    if (player_isTalking(game.players[i])) {
      noPlayersTalking = false;               
    }
  }

  // if all possible players have quit, quit 
  if (game.numPlayers == MaxPlayers && noPlayersTalking) {
    return true;
  }

  return false;       // keep looping                             

}

/* ******************** handlePlayMessage() ************************** */
/* Message-specific handler function for handleMessage(), used for adding a new player */
/*
* Caller provides: 
*   add_t from, address of the sender
*   const char* message, a message from the server
* We do:  
* if there are not already MaxPlayers that have joined the game:
*     get the name of the player from the start of message content after "PLAY "
*     verify the address and name of the sender
*     send "OK" message to player to confirm they joined game
*     add a player struct to game's player list and store information for this player 
*     call gridNewPlayer to generate a random position and a grid object for the player
*     update all players grids, send GRID and GOLD message to sender, send DISPLAY to all 
* if there are already MaxPlayers: 
*     send QUIT message to current sender
* We return: 
*   nothing 
*/ 
static void handlePlayMessage(const addr_t from, const char* message)
{
  const char* content = message + strlen("PLAY ");    // pointer to the middle of the message
  
  if (game.numPlayers < MaxPlayers)
  {
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
      sendErrorMessage(from, "Sorry - you cannot rejoin the same game.\n");
      return;
    }

    // send ok message to confirm player joined 
    sendOkMessage(from, letter);
    
    // store information about player 
    player_setName(game.players[game.numPlayers], name);
    player_setLetter(game.players[game.numPlayers], letter);
    player_changeStatus(game.players[game.numPlayers], true);
    player_setAddress(game.players[game.numPlayers], from);

    /// set random position and create empty grid for player 
    player_setGrid(game.players[game.numPlayers], gridNewPlayer(game.masterGrid, player_getLetter(game.players[game.numPlayers])));

    updateAllGrids();                                 // call updateGrid to recalculate visibility for all players

    sendGridMessage(from);                            // immediately send GRID and GOLD messages to new clients
    sendGoldMessage(0, 0, game.goldRemaining, from);
 
    sendDisplayToAll();                               // send DISPLAY message to all clients

    game.numPlayers++;                                // incremement the number of players that have joined so far 

  }
  else 
  {
    sendQuitMessage(from, "Game is full: no more players can join.\n"); // too many players
  }
}

/* ******************** handleSpectateMessage() ************************** */
/* Message-specific handler function for handleMessage(), used for adding a new spectator */
/*
* Caller provides: 
*   add_t from, address of the sender
*   const char* message, a message from the server
* We do:  
*   if we already have a spectator: 
*     compare address of sender to our spectator, replace and send QUIT message to old spectator if they are different 
*   verify the address is valid
*   set the spectators address to from
*   send GRID, GOLD, DISPLAY messages to new spectator
* We return: 
*   nothing 
*/ 
static void handleSpectateMessage(const addr_t from, const char* message)
{
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
  if (message_isAddr(from)){

    spectator_t* newSpectator = game.spectator;
    spectator_setAddress(newSpectator, from);

    sendGridMessage(from);

    sendGoldMessage(0, 0, game.goldRemaining, from);

    sendSpecDisplayMessage(from);
  }  

}

/* ******************** handleKeyMessage() ************************** */
/* Message-specific handler function for handleMessage(), used when a client presses a key */
/*
* Caller provides: 
*   add_t otherp, address of the sender
*   const char* message, a message from the server
* We do:  
*   check if we are dealing with a spectator or a player
*   if it is a player: 
*     get the letter of the player from its address
*     if key is 'q' or 'Q', quit player, update all grids, send DISPLAY message to all clients  
*     if key is one of the moving keys: call gridValidMove and handleMoveResult 
*     send ERROR message if invalid keystroke
*   if it is a spectator: 
*       if key is 'q' or 'Q', set spectators address to no address and send QUIT message to spectator
*       send ERROR message if invalid keystroke 
* We return: 
*   nothing
*/ 
static void handleKeyMessage(const addr_t otherp, const char* message)
{
  bool foundPlayer = false; 
  bool foundSpectator = false; 

  // check if this is a spectator 
  if (message_eqAddr(spectator_getAddress(game.spectator), otherp)) {
    foundSpectator = true; 
  }

  // loop through the list of players to check if it is a player 
  player_t* currPlayer;
  if ((currPlayer = findPlayer(otherp)) != NULL) {
    foundPlayer = true; 
  }

  const char* content = message + strlen("KEY ");        // pointer to message, starting after "KEY "
  char key; 
  sscanf(content, " %c", &key);                         // parse content for keystroke using sscanf 

  if (foundPlayer) {

    char letter = player_getLetter(currPlayer);
    int moveResult; 

    // switch value of key 
    switch(key) {
      case 'Q' : case 'q':

        sendQuitMessage(otherp, "Thanks for playing!\n");

        player_changeStatus(currPlayer, false);

        grid_deletePlayer(game.masterGrid, player_getLetter(currPlayer));     // delete player from master grid 

        updateAllGrids();

        sendDisplayToAll();

        break;

      case 'h': case 'l': case 'j': case 'k': case 'y': case 'u': case 'b': case 'n':

        moveResult = gridValidMove(game.masterGrid, letter, key);             // get result of attempted move 
        handleMoveResult(moveResult, currPlayer, otherp);

        break;

      case 'H': case 'L': case 'J': case 'K': case 'Y': case 'U': case 'B': case 'N':

        key = tolower(key);

        moveResult = gridValidMove(game.masterGrid, letter, key);            // get result of attempted move 
        handleMoveResult(moveResult, currPlayer, otherp);

        break;

      default: 
        sendErrorMessage(otherp, "Invalid keystroke\n");            // ignore invalid keystrokes and send ERROR message
    }
  }

  else if (foundSpectator) {

    // switch value of key  - spectator can only send 'Q' keystroke 
    switch(key) {
      case 'Q': case 'q':
        spectator_setAddress(game.spectator, message_noAddr());     // set spectator's address to no address
        sendQuitMessage(otherp, "Thanks for watching!\n");          // send QUIT message to spectator

        break;
    
      default: 
        sendErrorMessage(otherp, "Invalid keystroke\n");            // ignore invalid keystrokes and send ERROR message
    }
  }
}

/* ******************** handleMoveResult() ************************** */
/* Message-specific handler function for handleMessage(), used for adding a new spectator */
/*
* Caller provides: 
*   int moveResult, the result of gridValidMove 
*   currPlayer, the player struct for sender
*   addr_t otherp, the address of sender 
* We do:  
*   if the player successfully moved, update all grids and send DISPLAY messages to all clients
*   if the player's move was invalid, send ERROR message 
*   if the player stepped on gold:
*     add nuggets to players gold count
*     decrement from goldRemaining
*     update all grids and send DISPLAY messages to all clients
* We return: 
*   nothing 
*/ 
static void handleMoveResult(int moveResult, player_t* currPlayer, addr_t otherp) 
{
  
  if (moveResult == 0) {          // user successfully moved 

    updateAllGrids();

    sendDisplayToAll();

  }
  else if (moveResult == -1) {
    sendErrorMessage(otherp, "Player cannot make this move");
  }
  else {                          // if moveResult is nonzero, it represents # nuggets picked up
    player_addGold(currPlayer, moveResult);
    game.goldRemaining = game.goldRemaining - moveResult;

    updateAllGrids(); 

    sendGoldToAll(moveResult, currPlayer);

    sendDisplayToAll();
  }
}


/* ******************** sendOkMessage() ************************** */
/* Check parameters, construct and send OK message to given address */
static void sendOkMessage(const addr_t otherp, char letter)
{
  if (message_isAddr(otherp) && letter != SPACE) {

    char okMessage[message_MaxBytes];
    sprintf(okMessage, "OK %c", letter);

    message_send(otherp, okMessage);

  }
}

/* ******************** sendGridMessage() ************************** */
/* Check parameters, construct and send GRID message to given address */
static void sendGridMessage(const addr_t otherp)
{
  if (message_isAddr(otherp)) {

    // get number of rows and columns from masterGrid
    int numRows = getNumRows(game.masterGrid); 
    int numCols = getNumColumns(game.masterGrid);

    char response[message_MaxBytes];
    sprintf(response, "%s %d %d", "GRID ", numRows, numCols);

    message_send(otherp, response);
  }

}

/* ******************** sendGoldMessage() ************************** */
/* Check parameters, construct and send GOLD message to given address */
static void sendGoldMessage(int n, int r, int p, const addr_t otherp)
{
  if (message_isAddr(otherp) && n >= 0 && p >= 0 && r >= 0) {

    char response[message_MaxBytes];
    sprintf(response, "GOLD %d %d %d", n, r, p);

    message_send(otherp, response); 
  }

}

/* ******************** sendDisplayMessage() ************************** */
/* Check parameters, construct and send DISPLAY message to given address */
static void sendDisplayMessage(player_t* player, const addr_t otherp) 
{ 
  if (player != NULL && message_isAddr(otherp)) {

    char* grid1D = gridPrint(player_getGrid(player), player_getLetter(player));     // construct 1D grid with gridPrint

    char response[message_MaxBytes];
    sprintf(response, "DISPLAY\n%s", grid1D);

    printf("Server Display Grid: \n");
    printf("%s",grid1D);

    message_send(otherp, response);
    mem_free(grid1D);
  }
  
}


/* ******************** sendSpecDisplayMessage() ************************** */
/* Check parameters, construct and send DISPLAY message with masterGrid to spectator address */
static void sendSpecDisplayMessage(const addr_t otherp)
{
  if (message_isAddr(otherp)){

    char* grid1D = gridPrint(game.masterGrid, PERIOD);      // construct 1D grid of masterGrid with gridPrint

    char response[message_MaxBytes];
    sprintf(response, "DISPLAY\n%s", grid1D);

    message_send(otherp, response);
    mem_free(grid1D);
  }

}


/* ******************** sendErrorMessage() ************************** */
/* Check parameters, construct and send ERROR message to given address */
static void sendErrorMessage(const addr_t otherp, char* explanation)
{
  if (message_isAddr(otherp) && explanation != NULL) {

    char response[message_MaxBytes];
    sprintf(response, "ERROR %s", explanation); 

    message_send(otherp, response); 
  }
  
}

/* ******************** sendQuitMessage() ************************** */
/* Check parameters, construct and send QUIT message to given address */
static void sendQuitMessage(const addr_t otherp, char* explanation)
{
  if (message_isAddr(otherp) && explanation != NULL) {

    char quitMessage[message_MaxBytes];
    sprintf(quitMessage, "QUIT %s", explanation);

    message_send(otherp, quitMessage); 
  }
  
}

/* ******************** sendQuitMessage() ************************** */
/* Check parameters, construct and send QUIT message to given address */
static void sendDisplayToAll()
{
  // loop through players, send DISPLAY message to each one 
  for (int i = 0; i < MaxPlayers; i++) {

    player_t* thisPlayer = game.players[i];

    if (player_isTalking(thisPlayer) && message_isAddr(player_getAddress(thisPlayer))) {

      updateGrid(player_getGrid(thisPlayer), game.masterGrid, player_getLetter(thisPlayer));
      
      addr_t address = player_getAddress(thisPlayer);
    
      sendDisplayMessage(thisPlayer, address); 
    }
  }

  // send to spectator   
  addr_t specAddress = spectator_getAddress(game.spectator);
  
  if (message_isAddr(specAddress)){
    sendSpecDisplayMessage(specAddress);
  }
}

/* ******************** sendGoldToAll() ************************** */
/* Check parameters, construct and send GOLD message to all talking clients */
static void sendGoldToAll(int moveResult, player_t* currPlayer) 
{
  for (int i = 0; i< MaxPlayers; i++){

    player_t* player = game.players[i];
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
  // send to spectator 
  addr_t specAddress = spectator_getAddress(game.spectator);
  if (message_isAddr(specAddress)){
    sendGoldMessage(0, 0, game.goldRemaining, specAddress);
  }
}

/* ******************** updateAllGrids() ************************** */
/* Call updateGrid on every talking player with masterGrid */
static void updateAllGrids()
{
  for (int i = 0; i < MaxPlayers; i++) {

    player_t* player = game.players[i];

    if (player_isTalking(player)) {
      updateGrid(player_getGrid(player), game.masterGrid, player_getLetter(player));
    }
  }
}

/* ******************** findPlayer() ************************** */
/* Given an address, return the player in the list of players with that address, NULL otherwise */
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

/* ******************** deleteAllPlayers() ************************** */
/* Loop through player list and call player_delete on each player */
static void deleteAllPlayers()
{
  for (int i = 0; i < MaxPlayers; i++) {
    player_delete(game.players[i]);
  }
}

/* ******************** getName() ************************** */
/* Given a name, parse, truncate, and return */
/*
* We do: 
*   loop through content pointer
*   truncate an over-length real name to MaxNameLength characters
*   replacing with an underscore _ any character for which both isgraph() and isblank() are false
*/
static char* getName(const char* content)
{
  char* name = mem_malloc_assert(sizeof(char)*(MaxNameLength + 1), "Unable to allocate memory for player name.\n");

  // loop through content pointer 
  int i; 
  for (i = 0; i < strlen(content) && i < MaxNameLength; i++){

    if ( (!isgraph(content[i])) && !(isblank(content[i])) ) {
      name[i]='_';
    }

    else {
      name[i] = content[i];
    }

  }
  name[i] = '\0';             // truncate at MaxNameLength

  return name; 
}

/* ******************** gameOver() ************************** */
/* Delete game struct, construct GAME OVER message, send to all talking clients */
/*
* We do: 
*   construct a GAME OVER message with a tabular summary of all past players' names, scores, and letters 
*   broadcast GAME OVER message to all talking clients 
*   delete all player structs by calling player_delete, which calls gridDelete on each players grid 
*   free player list 
*   delete masterGrid by calling gridDelete, which frees all grid memory 
*   delete spectator if it exists 
*/
static void gameOver()
{
  // construct and broadcast game over message
  char gameOverMessage[message_MaxBytes];
  sprintf(gameOverMessage, "GAME OVER:\n");

  for (int i = 0; i < MaxPlayers; i++) {

    player_t* p = game.players[i];

    if (strcmp(player_getName(p),"")!=0){

      // get summary for each player and print to gameOverMessage
      char* info;
      for (info = gameOverMessage ; *info ; info++);

      char letter = toupper(player_getLetter(p));
      sprintf(info, "%c\t%3d %s\n", letter, player_getGold(p), player_getName(p));
      } 
  }
  
  // broadcast game over message to all players who are still talking to server 
  for (int i = 0; i < MaxPlayers; i++) {

    player_t* player = game.players[i];

    if (player_isTalking(player) && message_isAddr(player_getAddress(player))) {
      sendQuitMessage(player_getAddress(player), gameOverMessage);
    }
  }

  // send spectator game over message 
  addr_t specAddress = spectator_getAddress(game.spectator);

  if (message_isAddr(specAddress)){
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





