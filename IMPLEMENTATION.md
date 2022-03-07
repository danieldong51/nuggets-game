# CS50 Nuggets
## Implementation Spec
### Team Tux, Winter 2022

According to the [REQUIREMENTS.md](https://github.com/cs50winter2022/nuggets-info/blob/main/REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes the player, spectator, and grid modules. 
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

<<<<<<< HEAD
- Emily: Grid module, minus visualizer function
- Daniel: Visualizer, Server
- Georgia: Player, Server
- Jeff: Client
- We all do: Testing, Documentation
=======
- Emily and Dan: grid 
- Georgia: server, player  
- Jeff: spectator, client 
- We all do: documentation, testing 
>>>>>>> 867a9a232099efabb7a1622b9ce95f1af062cca3


## Client

### Data structures

The data structures for the client are the _game_ struct, which is a single global variable accessible to both the client and server, and the _score_ struct, which is a struct that stores information about a player's purse. The structs are defined like so:

```c
<<<<<<< HEAD
struct game {
  int goldRemaining;              // amount of gold left in game 
  int numPlayers;                 // amount of players that have joined so far 
  player_t** players;             // list of player structs 
  grid_t* masterGrid;             // master grid struct
  spectator_t* spectator;         // spectator struct
  int seed;                       // seed for rand()
} game; 
=======
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
>>>>>>> 867a9a232099efabb7a1622b9ce95f1af062cca3
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

Functions to construct and send `QUIT`, `OK`, `GRID`, `ERROR`, `GOLD`, and `DISPLAY` messages to given sender. 

```c
static void sendOkMessage(const addr_t otherp, char letter);
static void sendQuitMessage(const addr_t otherp, char* explanation);
static void sendGridMessage(const addr_t otherp);
static void sendErrorMessage(const addr_t otherp, char* explanation);
static void sendGoldMessage(int n, int r, int p, const addr_t otherp);
static void sendDisplayMessage(player_t* player, const addr_t otherp);
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

### Data structures

#### position
```c
typedef struct position position_t; 
```
Contains
* x coordinate
* y coordiante

#### pile
```c
typedef struct pile pile_t;
```
Contains
* position of gold pile 
* amount of gold in that pile


#### playerAndPosition
```c
typedef struct playerAndPosition playerAndPosition_t;
```
Contains
* position of the player 
* amount of gold in that pile


#### grid
```c
typedef struct grid grid_t;
```
This structure contains majority of data and variables necessary for nuggets gamplay.
Contains
* array of strings, with each array respresenting a row of a map
* array of playerAndPosition structures
* array of pile structures
* number of columns in the map
* number of rows in the map


### Definition of function prototypes

A function to convert a map file into a 2d Array representing the map.

```c
void gridConvert(char** grid, FILE* fp, int nrows, int ncols)
```
A function that converts map.txt file to a grid.

```c
void updateGrid(grid_t* playerGrid, grid_t* masterGrid, char playerLetter)
```
A function that updates a player's grid based on the player's location in the master grid

```c
char* gridPrint(grid_t* playerGrid, char playerLetter)
```
A function that prints a grid with locations the players and gold piles stored within the grid

```c
int gridValidMove(grid_t* masterGrid, char playerLetter, char moveLetter)
```
A function that checks if a move command is valid, given the move command letter, If the move is valid, it will update the player's position in the master grid given the player letter. Returns an integer based on characteristics of the move. 

```c
grid_t* grid_new();
```
A function that allocates space for an empty grid structure

```c
void gridMakeMaster(grid_t* masterGrid, char* fileName, int numGold, int minGoldPiles, int maxGoldPiles, int randInt)
```
A function fills up to gold pile array and the 2d string array (representing map) of a grid object--only needed when making the masterGrid, which the server needs

```c
grid_t* gridNewPlayer(grid_t* masterGrid, char playerLetter)
```
A function that returns new playerGrid for player, which starts off as completely empty. The function also creates a new playerAndPosition struct representing new player in the masterGrid

```c
int getNumRows(grid_t* masterGrid)
```

```c
int getNumColumns(grid_t* masterGrid)
```

```c
int getGrid2D(grid_t* masterGrid)
```

```c
void gridDelete(grid_t* map, bool isMaster)
```

```c
void grid_deletePlayer(grid_t* masterGrid, char playerLetter)
```

### Detailed pseudocode

#### `gridConvert`
```
Take in file
Obtain # of rows in file
Create array of strings with (# of row) slots
For number of rows in file
	Add contents of row x into slot x of the array of strings
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
Create a new grid with the same dimensions as the grid that needs to br printed
for each row in the new grid
	copy contents of old grid into the new grid
if the array of playerAndPosition structures in the original grid is not null
	for each player
		if the playerAndPosition structure is not null, and the position is not null
			if the position is the current player's position
				set the position in the new grid to be '@'
			else
				set the position in the grid to be the player's letter
if the array of pile structures in the original grid is not null 
	for each pile
		mark the position of the pile in the new grid to be '*'

create a new string that represents all characters in the new grid
	for each row in the new grid
		for each column in the new grid
			copy the contents of the new grid into the apprporiate index of the new string
return the new string
```

#### `gridValidMove`
```
given player letter, calculate the index of the letter in the playerAndPosition array in the amsterGrid
if the playerAndPosition structure is not null
	store the x and y coridnates of the positon
if move letter is h
		move the (x,y) to the left
if move letter is l
		move the (x,y) to the right
if move letter is j
	move the (x,y) up
if move letter is k
	move the (x,y) down
if move letter is y
	move the (x,y) diagonally up and left
if move letter is u
	move the (x,y) diagonally up and right
if move letter is b
	move the (x,y) diagonally down and left
if move letter is n
	move the (x,y) diagonally down and right
	
if the coordinates are out of bounds of the map	
	return -1
	
if the cordinates after the move are an empty room spot or passage spot in the map
	return 0
if the cooridanates after the move are a spot with a gold pile in the map
	return the amount of gold in the gold pile
else
	return -1
```

#### `gridMakeMaster`
```
open the map file
calculate number of rows and columns in file
set nrows in masterGrid
set ncols in mastergrid

create a new array of strings
call gridConvert on the new array of strings
set grid2D in masterGrid to the the converted array of strings

set random number of gold piles give the maxGoldPiles and minGoldPiles
create an array of piles representing the gold piles

for each goldPile
	set a random position on the map for the gold pile (must be in an originally empty space)
	set a random amount of gold to be in the pile using rand()

for each goldPile
	scale down the amount of gold in each pile so that the total gold placed in the map is equal to numGold

set the masterGrid's goldPiles to the goldPiles just created
```

#### `gridNewPlayer`
```
create random position in the map to be the postiion of the player (must be in an originally empty space)
creat a new playerAndPosition structure with the random position and character's letter
add the new playerAndPosition structure to the masterGrid's list of playerAndPosition structures

create a new playerGrid, initialized to empty
create the grid2D for the player grid, initalized to empty, with the same dimensions of the masterGrid
set nrows and ncols for the playerGrid to be the same as masterGrid
```

#### `gridDelete`
```
if the grid is the masterGrid
	free all the piles in the array of piles
	free the array of piles
	free all the playerAndPositions in the array of playerAndPositions
	free the array of playerAndPositions

free the first pointer to the array of strings, grid2D[0]
free the array of strings, grid2D

free the map
```

#### `grid_deletePlayer`
```
find the index of the player in the masterGrid based on the players letter
set the player in the masterGrid's array of playerAndPostiions
	if the playerAndPosition structure is not null
		free the position
		free the playerAndPosition
		set the index in the array to be null
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


