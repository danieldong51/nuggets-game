# CS50 Nuggets
## Implementation Spec
### Team Tux, Winter 2022

According to the [REQUIREMENTS.md](https://github.com/cs50winter2022/nuggets-info/blob/main/REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes the player, spectator, and grid modules. 
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

- Emily and Dan: grid 
- Georgia: server, player  
- Jeff: spectator, client 
- We all do: documentation, testing 


## Client

### Data structures

The data structures for the client are the _game_ struct, which is a single global variable accessible to both the client and server, and the _score_ struct, which is a struct that stores information about a player's purse. The structs are defined like so:

```c
static struct {
  char hostname[messageMaxLength];
  char port[maxPortLength];
  addr_t serverAddress;
  char playerName[messageMaxLength];
} game;
```
```c
typedef struct score {
  char letter;
  int collected;
  int nuggetsLeft;
  int purse;

} score_t;
```

### Definition of function prototypes


A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```

A function to handle input from stdin. handleInput is used as a helper function for message_send(), which the client uses to send messages to the server. 

```c
bool handleInput(void* arg);
```

A function to handle inbound messages from the server. handleMessage is used by the client to receive messages to the server. 

```c
bool handleMessage(void* arg, const addr_t from, const char* message);
```

### Detailed pseudo code

#### `parseArgs`:
```
  validate command-line arguments
  check if hostname or IP address is valid address by calling message_isAddr()
  initialize message module by calling message_init() 
  print assigned port number
  decide whether spectator or player
```

#### `handleInput`:
```
Parse stdin using ncurses.
```

#### `handleMessage`:
```
if the message from the server is: 
  “OK”:
    User was allowed into game
  “GRID”:
    This message shows the GRID’s dimensions.
  “GOLD”:
    Allows user to see nuggets collected, current score, and nuggets left.
  “DISPLAY”:
    Allows the user to see the map.
  “QUIT”:
    Ends the ncurses library
  “ERROR”: 
    Print explanation that should be to the right of the error message.
```

## Server

### Data structures

The _game_ struct, which is a single global variable accessible to both client and server. The struct is defined as follows: 

```c
struct game {
  int goldRemaining;              // amount of gold left in game 
  int numPlayers;                 // amount of players that have joined so far 
  player_t** players;             // list of player structs 
  grid_t* masterGrid;             // master grid struct
  spectator_t* spectator;         // spectator struct
  int seed;                       // seed for rand()
} game; 
```

### Definition of function prototypes

A function to parse the command-line arguments, initialize the game struct, initialize the message module, call `srand()`.

```c
static int parseArgs(const int argc, char* argv[]);
```

A function to set up the data structures for the game, including loading the map.txt into a grid, saving it in our _game_ struct, and dropping piles of gold on the grid. 

```c
static void initializeGame(char* mapPathname);
```

A function to read the input message from the client and respond accordingly. Passed into `message_loop()` in main. 

```c
bool handleMessage(void* arg, const addr_t from, const char* message);
```

A function to end the game.

```c 
void gameOver() 
```

A function to handle a `PLAY` message from a client.

```c
static void handlePlayMessage(const addr_t from, const char* message);
```

A function to handle a `SPECTATE` message from a client. 

```c
static void handleSpectateMessage(const addr_t from, const char* message);
```

A function to handle a `KEY` message from a client. 

```c
static void handleKeyMessage(const addr_t otherp, const char* message);
```

A function to handle the outcome of an attempted move by a player. 

```c
static void handleMoveResult(int moveResult, player_t* currPlayer, addr_t otherp);
```

A function to return a player in _game.players_ with the given address. 

```c
static player_t* findPlayer(const addr_t address);
```

A function to get the name of a player given the content of its `PLAY` message. 

```c
static char* getName(const char* content);
```

A function to delete all players from _game.players_.

```c
static void deleteAllPlayers();
```

A function to update the every player's grid by calling updateGrid with that player's grid, letter, and the master grid. 

```c
static void updateAllGrids();
```

A function that calls `sendDisplayMessage` all players in _game.players_ and the spectator.

```c
static void sendDisplayToAll();
```

A function to call `sendGoldMessage` to all players in _game.players_ given the player who just picked up gold and the amount of gold they picked up.

```c
static void sendGoldToAll(int moveResult, player_t* currPlayer);
```

A function to construct and send a `QUIT` message to given address. 

```c
static void sendQuitMessage(const addr_t otherp, char* explanation);
```

Functions to construct and send `QUIT`, `OK`, `GRID`, `ERROR`, `GOLD`, and `DISPLAY` messages to given sender. 

```c
static void sendOkMessage(const addr_t otherp, char letter);
```c
static void sendGridMessage(const addr_t otherp);
```c
static void sendErrorMessage(const addr_t otherp, char* explanation);
```c
static void sendGoldMessage(int n, int r, int p, const addr_t otherp);
```c
static void sendDisplayMessage(player_t* player, const addr_t otherp);
```c
static void sendSpecDisplayMessage(const addr_t otherp);
```


### Detailed pseudo code

#### `main`:
```c
validate command line arguments using parseArgs()
call initializeGame() with map.txt 
initialize ‘message module’ 
print the port number on which we wait 
call message_loop()
call gameOver() to inform all clients the game has ended
clean up
```

#### `parseArgs`:
```c
validate number of arguments 
verify map file can be opened for reading
if seed provided
  verify it is a valid seed number
  seed the random-number generator with that seed
else
  seed the random-number generator with getpid()
```

#### `initializeGame`:
```c
initialize information about the game
initialize list of players by calling player_new() for each player
call gridMakeMaster() with mapPathname and game.masterGrid to initialize and drop gold on game grid 
initialize spectator
```


#### `handleMessage`:
```c
if the messsage from the client is: 
	“PLAY”
    call handlePlayMessage 
	“SPECTATE”
    call handleSpectateMessage
  "KEY"
    call handleKeyMessage 
	otherwise, send ERROR message for uknown command 
if there is no more gold, return true
if no more players are talking, return true 
return false 
```

#### `handlePlayMessage`:
```c
if there are not already MaxPlayers that have joined the game
  get the name of the player from the start of message content after "PLAY "
  verify the address and name of the sender
  send "OK" message to player to confirm they joined game
  add a player struct to game.players and store information for this player 
  call gridNewPlayer to generate a random position and a grid object for the player
  update all players grids
  send GRID message to sender
  send GOLD message to sender
  send DISPLAY message to all clients
  increment the number of players that have joined so far 
if there are already MaxPlayers:
  send QUIT message to current sender
```

#### `handleSpectateMessage`:
```c
if we already have a spectator: 
  compare address of sender to our spectator, replace and send QUIT
message to old spectator if they are different 
verify the address is valid
set the spectators address to from
send GRID, GOLD, DISPLAY messages to new spectator
```

#### `handleKeyMessage`:
```c
check if we are dealing with a spectator or a player
if it is a player: 
  get the letter of the player from its address
  if key is: 
  'q' or 'Q': 
    send quit message to player 
    change players status to not talking
    delete player from master grid
    update all grids
    send DISPLAY message to all clients 
  one of the moving keys: 
    call gridValidMove on key to get moveResult
    call handleMoveResult to send messages to clients based on outcome of attempted move 
if it is a spectator: 
  if key is: 
  'q' or 'Q':
    set spectators address to no address 
    send QUIT message to spectator
    send ERROR message if invalid keystroke 
```


#### `handleMoveResult`:
```c
if moveResult == 0:
  user successfully moved, so update all grids 
  send DISPLAY message to all clients 
if moveResult == -1: 
  move was invalid, send ERORR message to sender
else: 
  nonzero moveResult represents amount of gold picked up by player 
  add gold to this players numGold
  decrement from goldRemaining
  update all grids
  send DISPLAY message to all clients
```

#### `findPlayer`:
```c
if the given address is valid: 
  for each player in game.players: 
    if player address matches given adress, return player
  return null
```

#### `deleteAllPlayers`:
```c
for each player in game.players
  call player_delete()
```

#### `getName`:
```c
loop through given pointer to message content until MaxNameLength
if a character has both isgraph() and isblank() false:
  replace with an underscore _ 
truncate name 
return name 
```

### `sendQuitMessage`, `sendOkMessage`,  `sendGridMessage`, `sendGoldMessage`, `sendDisplayMessage`, `sendSpecDisplayMessage`

```c
check that addres is valid and verify other parameters if necessary
create a response string of message_MaxBytes length
construct specified message
send message to given address
```

---

## Grid
The `grid` module is used to update the user interface for all clients each time a client sends a message.

### Data structure

Position
```c
typedef struct position position_t; 
```
Gives (x, y) coordinate position


Pile
```c
typedef struct pile pile_t;
```
Gives coordinate of gold pile and the amount of gold in that pile


PlayerAndPosition
```c
typedef struct playerAndPosition playerAndPosition_t;
```
Contains position of the player and the players name


Grid
```c
typedef struct grid grid_t;
```
Contents 2D string array representing the map, list of piles representing the gold piles within the map, and list of positions representing the positions of the players


### Definition of function prototypes

A function to convert a map file into a 2d Array representing the map.

```c
gridConvert(file *fp)
```

A function to update the player's grid based on the serverGrid and the other players. 

```c
bag_t* updateGrid(grid_t* playerGrid, grid_t* serverGrid);
```

A function that takes a grid and the current position of a player and prints the appropriate grid for that player.

```c
Void gridPrint(grid_t* grid, currentPosition) 
```


A function that takes a coordinate and checks if the position is an open space to move to.

```c
int gridValidMove(position_t* coordinate)
```

### Detailed pseudocode

#### `gridConvert`
```
Take in file
Obtain # of rows in file
Create array of strings with (# of row) slots
For number of rows in file
	Add contents of row x into slot x of a the array of strings
```

#### `updateGrid`
```
	initialize toVisit bag
	initialize char** visited as a blank grid with same dimensions as serverGrid
	clear players and piles of gold in playerGrid
	Add adjacent squares to toVisit bag
	Mark adjacent squares in visited
	While toVisit is not empty:
		extract square from toVisit
		if toVisit isVisible:
			mark square as visible:
				add player or piles of gold to grid struct if it exists at that square
			loop over adjacent squares:
				if square not visited:
          mark square as visited
          add to toVisit
```

#### `gridPrint`
```
For i < # of players, i++
  Obtain position of player (x, y)
  If (x,y) = currentPostion
    Grid[y][x] = “@”
  Else
    Grid[y][x] = “i+65” (ascii conversion in character form) 
  For i < # of gold piles, i++
    Obtain position of pile, (x, y)
    Grid[y][x] = “*”
  For i<slots in grid array
    Print the string in a new line
```

#### `gridValidMove`
```
	Given position (x,y)
	If grid[y][x] = ‘*’ // gold pile
		For each pile list of piles in grid
			If position of pile of grid equals (x,y)
				Return amount in that pile of code
		Return 
	If grid[y][x] = ‘#’ or  ‘.’
		Return 1
	Return 0
```

---

## Player 

### Data structures

The data structures for the player are the player’s name, which is a char* pointer, its location, which is represented with the _position_ struct, the amount of gold the player has, which is an integer, the rooms it has seen before, which is represented by the _grid_ struct, and whether the player is currently talking to the server, which is a boolean. Every player also has an _addr_, which represents its address, and a letter which represents its index in the _game.players_ list.  

```c
char* name; 
char letter;
int numGold; 
grid_t* grid;
bool isTalking; 
addr_t address;
```

### Definition of function prototypes

A function to create a new player struct. It initializes the players information to empty. 

```c
player_t* player_new(char* name, char* letter, position_t* pos); 
```

A function to delete a player and free all of its memory. 

```c
void player_delete(player_t* player); 
``` 

A function that returns whether or not a player is currently talking to the server. 

```c
bool player_isTalking(player_t* player); 
```

A function to return the grid object of a player.

```c 
grid_t* player_getGrid(player_t* player); 
```

A function to return the address of a player. 

```c 
addr_t player_getAddress(player_t* player);
```

A function to return the name of a player. 

```c 
char* player_getName(player_t* player); 
``` 

A function to return the letter of a player. 

```c 
char player_getLetter(player_t* player);
```

A function to increment the number of gold a player has by _numGold_ amount. This function is called by the server when a player steps on a gold pile. 

```c
void player_addGold(player_t* player, int numGold);
```

A function to change whether or not the player is talking to the server. 

```c 
void player_changeStatus(player_t* player); 
```

A function to set letter of a player. 

```c 
void player_setLetter(player_t* player, char letter);
```

A function to set name of a player. 

```c 
void player_setName(player_t* player, char* name);
```

A function to set grid of a player. 

```c 
void player_setGrid(player_t* player, grid_t* grid);
```

A function to set address of a player. 

```c 
void player_setAddress(player_t* player, addr_t address); 
```
 
### Detailed pseudo code

#### `player_new`

```c
create memory for new player struct
initialize player name to empty string, player letter to ' '
initialize numGold to zero and grid to null
initialize address to message_noAddr()
initialize player isTalking to false
return player
```

#### `player_delete` 

```c
if the given player struct is not null
	if grid is not null 
    call gridDelete on grid 
  if player name is not empty
    free player name 
  free player memory 
```

#### `player_isTalking`
```c
if player is not null 
	return this player’s bool struct isTalking
else 
	return false 
```

#### `player_getGrid`

```c
if player is not null 
	return this player’s grid struct 
else 
	return null
```

#### `player_getGrid`

```c
if player is not null
  return this players grid
return null 
```

#### `player_getAddress`

```c
if player is not null
  return this players address
return null 
```


#### `player_addGold` 

```c
if player is not null and numGold>0 
	add the given amount to this player’s numGold count
```

#### `player_changeStatus`

```c
if player is not null 
  set isTalking to given value
else 
  do nothing
```

#### `player_setLetter`

```c
if player and letter are not null 
  set this player letter to given value
else 
  do nothing
```

#### `player_setName`

```c
if player and name are not null 
  set this player name to given value
else 
  do nothing
```

#### `player_setGrid`

```c
if player and grid are not null 
  set this player grid to given value
else 
  do nothing
```

#### `player_setAddress`

```c
if player is not null and address is valid
  set this player address to given value
else 
  do nothing
```

---

## Spectator

### Data structures

The data structure for the spectator is the rooms it has seen before, which is represented by the _grid_ struct.

```c
grid_t* grid;
```

### Definition of function prototypes

A function to create a new spectator struct. 

```c
spectator_t* spectator_new(grid_t* masterGrid); 
```

A function to delete a spectator and free all of its memory. 

```c
void spectator_delete(spectator_t* spectator); 
``` 

A function to return the grid object of a spectator.

```c 
grid_t* spectator_getGrid(spectator_t* spectator);
```

A function to return the address of a spectator. 

```c 
addr_t spectator_getAddress(spectator_t* spectator);
```

A function to set grid of a spectator. 

```c 
void spectator_setGrid(spectator_t* spectator, grid_t* grid);
```

A function to set address of a spectator. 

```c 
void spectator_setAddress(spectator_t* spectator, addr_t address);
```

### Detailed pseudo code

#### `spectator_new`

```
if the spectator given is not NULL,
	 set the grid for the player to be an empty grid
 return the spectator.
else:
	Print an error message and return NULL.
```

#### `spectator_delete` 

```
if the given spectator struct is not null:
	free the memory for the spectator object
```

#### `spectator_getGrid`

```c
if spectator is not null
  return this spectator grid
return null 
```

#### `spectator_getAddress`

```c
if spectator is not null
  return this spectator address
return null 
```

#### `spectator_setGrid`

```c
if spectator and grid are not null 
  set this spectator grid to given value
else 
  do nothing
```

#### `spectator_setAddress`

```c
if spectator is not null and address is valid
  set this spectator address to given value
else 
  do nothing
```

---

## Testing plan

### unit testing

We will conduct unit tests according to detailing in the Design Spec, [DESIGN.md](https://github.com/cs50winter2022/nuggets-team-tux/blob/submit-implementation/DESIGN.md). We will individually test each module as we code to ensure each individual unit of the program runs smoothly. 

### integration testing

We will test the server and the client programs extensively, including: 
- End game behavior 
- More than MaxPlayers players trying to connect
- More than one spectator trying to connect
- Testing invalid server address on the client side 
- Players all quitting
- All gold collected

Finally, we will run `client` and `server` on a Thayer Linux server by giving the server's hostname on the client's command line. We’ll run a server on `babylon5` and a client on `plank`. 

We will test both ‘client’ and ‘server’ using the four programs in the shared directory `~/cs50-dev/shared/nuggets/`. 

### system testing

We will test the client and server together, looking to check:
The server can take connections
The client can connect to the server
The server can hold up to 26 clients and 1 spectator
The server remembers which player a client was, and fills them in as needed

---

## Limitations

N/A at the moment. 


