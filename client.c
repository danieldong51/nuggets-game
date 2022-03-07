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
#include <ncurses.h>

#define messageMaxLength 65507
#define maxPortLength 6

//constants
const int maxStatusLength = 48;

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
  int collected;
  int nuggetsLeft;
  int purse;

} score_t;


static const int MaxNameLength = 50; //max characters in playerName
//bool screenSizeOK = false; //whetehr screen is big enough
static int ncols, nrows; //number of columns and rows of the map

/**************** file-local functions ****************/
void initNcurses();
bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
void handleQuit(const char* message);
void handleGrid(const char* message);
void handleGold(const char* message);
void handleDisplay(const char* message);
void handleError(const char* message);
void mapDisplay(char* map);





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

/****************** initCurses ***********
 *
 */
void initNcurses() {
  
  initscr();

  cbreak();
  noecho();

  //change the color of the screen
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  attron(COLOR_PAIR(1));
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
   if (strncmp(message, "OK ", strlen("OK ")) == 0){
    sscanf(message, "OK %c", &game.score->letter); 
   }

   else if (strncmp(message, "GRID ", strlen("GRID ")) == 0){
     handleGrid(message);
   }

   else if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0){
     handleGold(message);
   }

    else if (strncmp(message, "DISPLAY", strlen("DISPLAY")) == 0){
      handleDisplay(message);
    }

    else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
      handleQuit(message);
      return true;
    }

    else if (strncmp(message, "ERROR ", strlen("ERROR ")) == 0){
      handleError(message);
    }
}



/***************handleQuit*****************/
void
handleQuit(const char* message){
  char copy[message_MaxBytes];
  strcpy(copy, message);

  char* messageQuit = copy + strlen("QUIT ");
  nocbreak();
  endwin();
  printf("%s", messageQuit);

}


/************handleGrid******************/
void
handleGrid(const char* message){
  int screenWidth, screenHeight;
  getmaxyx(stdscr, screenHeight, screenWidth);
  
  // read the grid message

  //if screen is big enough, continue
  while (screenWidth<nrows || screenHeight<ncols) {
    mvprintw(0, 0, "your screen is too small, please make it larger\n");
    mvprintw(1, 0, "press enter when ready\n");
    refresh();

    char c = getch();

    if (c == '\n') {
      getmaxyx(stdscr, screenHeight, screenWidth);
    }
  }

  clear();
  refresh();

}


/*********handleGold****************/
void
handleGold(const char* message){


  if (game.playername[0] == '\0') {
     mvprintw(0, 0, "Spectator: %d nuggets unclaimed.", game.score->nuggetsLeft);
  }
  else {
    mvprintw(0, 0, "Player %c has %d nuggets (%d nuggets unclaimed)",game.score->letter, game.score->purse, game.score->nuggetsLeft);
    if (game.score->collected > 0) {  
      mvprintw(0, maxStatusLength + 2, "GOLD received: %d",game.score->collected);
    }

  }

}


/*********handleDisplay***********/
void
handleDisplay(const char* message);


/********handleError*************/
void
handleError(const char* message){
  

}



/********* mapDisplay **********/
void mapDisplay(char* map) {

  int X = 0;
  int Y = 2;

  //for each character in map
  for (int i=0; i < strlen(map); i ++) {

    //Use switch statements for the following map characters
    switch(map[i]){

      //solid rock - interstitial space outside rooms
      case ' ' :

      //- a horizontal boundary
      case '-' :

      //| a vertical boundary
      case '|' :

      //+ a corner boundary
      case '+':

      //. an empty room spot
      case '.':

      //# an empty passage spot
      case '#':

      //the player
      case '@':

      //a pile of gold
      case '*':

      //for everything else
      default :
        if (isalpha(map[i]) != 0) {
          attron(COLOR_PAIR(1));
          mvprintw(Y, X, "%c", map[i]);
          positionNext(&Y, &X);

          attroff(COLOR_PAIR(1));

        }

        else{

        }

        break;


    }
  }

}


/***********positionNext*********/
void positionNext(int* Y, int* X) {


}
