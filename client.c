/* 
 * client - a simple client using the messaging module
 *
 * Given the address of a server, the client sends each line of stdin
 * as a message to the server, and prints to stdout every message received
 * from the server; each printed message is surrounded by 'quotes'.
 * 
 * Team tux - March 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include "message.h"

#define messageMaxLength 65507
#define maxPortLength 6

/*
 *
 * Global variables
 */
static struct {
  char hostname[messageMaxLength];
  char port[maxPortLength];
  addr_t serverAddress;
  char playerName[messageMaxLength];
} game;


typedef struct score {
  char letter;
  int goldRemaining;
} score_t;


static const int MaxNameLength = 50;

/**************** file-local functions ****************/
static bool handleInput  (void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);





/***************** main *******************************/
int
main(const int argc, char* argv[]){

  //argument check
  if (argc > 4 || argc<3) {
        fprintf(stderr,"Invalid number of arguments. Must be in the form ./player hostname, port, [playername]");
    }

  else{
    strcpy(game.hostname, argv[1]);
    strcpy(game.port, argv[2]);
  

  //initialize messaging system

  message_init(stderr);

  //set the address
  message_setAddr(game.hostname, game.port, &game.serverAddress);


  //The client is a spectator if there is no third argument
  if (argc == 3) {
     message_send(game.serverAddress, "SPECTATE");
  }

  else if (argc==4) {
    //check the length
    if (strlen(argv[3])>MaxNameLength) {
      strncpy(game.playername, argv[3], MaxNameLength);
    }

    else {
      strcpy(game.playername, argv[3]);
    }
    //send a message in the format "PLAY [playername]"
    char* message = malloc(strlen("PLAY")+ 51);
    sprintf(message, "PLAY %s", game.playername);
    message_send(game.serverAddress, message);
    free(message);


  }
  message_loop(NULL, 0, NULL, handleInput, handleMessage);
  }
}



/**************** handleInput ****************/
/* stdin has input ready; read a line and send it to the client. 
*/

bool
handleInput(void* arg){

  //player input
  char c = getch();
  char keyMessage[strlen("KEY " + 2)];

  sprintf(keyMessage, "KEY %c", key);           
  message_send(game.serverAddress, keyMessage);

  //Not complete
}


/**************** handleMessage ****************/
/* Datagram received; print it.
 * We ignore 'arg' here.
 * Return true if any fatal error.
 */
static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
   if (strncmp(message, "OK ", strlen("OK ")) == 0);

   else if (strncmp(message, "GRID ", strlen("GRID ")) == 0);

   else if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0);

    else if (strncmp(message, "DISPLAY", strlen("DISPLAY")) == 0);

    else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0);

    else if (strncmp(message, "ERROR ", strlen("ERROR ")) == 0);
}



/***************handleQuit*****************/
void
handleQuit(const char* message);


/************handleGrid******************/
void
handleGrid(const char* message);

/*********handleGold****************/
void
handleGold(const char* message);


/*********handleDisplay***********/
void
handleDisplay(const char* message);


/********handleError*************/
void
handleError(const char* message);



