# CS50 Nuggets
## Implementation Spec
### Team Tux, Winter 2022


According to the [REQUIREMENTS.md](https://github.com/cs50winter2022/nuggets-info/blob/main/REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes the player, spectator, and grid modules. 
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

- Emily: Grid module, minus visualizer function
- Daniel: Visualizer, Client
- Georgia: Player
- Jeff: Spectator
- We all do: Server, Testing, Documentation


## Client

### Data structures

The only data structure for the client is the _game_ struct, which is a single global variable accessible to both the client and server. The struct is defined like so: 

```c
typedef struct game { 
int maxNameLength = 50; 		// max number of chars in playerName
int maxPlayers = 26;      		// maximum number of players
int goldTotal = 250;     			// amount of gold in the game
int goldMinNumPiles = 10; 		// minimum number of gold piles
int goldMaxNumPiles = 30; 		// maximum number of gold piles

int goldRemaining;              // amount of gold left in game 
player_t* players[26];          // list of players 
grid_t* masterGrid;             // master grid object that contains master char 2D array + other info 

} game_t; 
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

The _game_ struct, which is a single global variable accessible to both client and server. The struct is provided in the client section of the Implementation Spec. 

### Definition of function prototypes

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```

A function to set up the data structures for the game, including loading the map.txt into a grid, saving it in our `game` struct, and dropping piles of gold on the grid. 

```c
static void initializeGame(char* mapPathname);
```

A function that calls message_loop to accept messages from the client and take the appropriate action. 

```c
static void acceptMessages(int port);
```

A function to read the input message from the client and respond accordingly.

```c
bool handleMessage(void* arg, const addr_t from, const char* message);
```

A function to end the game.

```c 
void gameOver() 
```

### Detailed pseudo code

#### `main`:
```
validate command line arguments using parseArgs()
call initializeGame() with map.txt 
initialize ‘message module’ 
print the port number on which we wait 
call acceptMessages()
call gameOver() to inform all clients the game has ended
clean up
```

#### `parseArgs`:
```
validate command-line arguments
verify map file can be opened for reading
if seed provided
  verify it is a valid seed number
  seed the random-number generator with that seed
else
  seed the random-number generator with getpid()
```

#### `initializeGame`:
```
	populate the global variable “game” with the given map.txt file by calling gridConvert()
	while we have not yet dropped GoldMaxNumPiles piles: 
		if we have dropped at least GoldMinNumPiles piles: 
			break 
		generate a random number, x
		move through the char* grid pointer x times or until you reach a valid room spot
		generate a random number, y, to determine the size of the pile 
		drop a pile of size y on this room spot 
```

#### `acceptMessages`:
```
print the port number of the client sending the message
call message_loop() with handleMessage() to await clients
call visibility 
```

#### `handleMessage`:
```
if the messsage from the client is: 
	“PLAY”
		if max players,
			Respond with “NO”
		else:
			Respond with “OK”. Means, user was allowed, add that player with player_new
	“SPECTATE”
		if there is a spot available for spectator,
			Add the spectator by calling spectator_new 
		else:
			Replace the spectator
	“KEY”
		Parse key stroke
		Make the appropriate move
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

	Take in file
	Obtain # of rows in file
	Create array of strings with (# of row) slots
	For number of rows in file
		Add contents of row x into slot x of a the array of strings

#### `updateGrid`

	initialize empty visible grid
	while there are still visible squares in the loop:
		increment radius
		go around the perimeter of the square of radius by radius
		if square is visible:
			mark square as visible
	clear playergrid's playerlist
	loop over player positions in mastergrid:
		if player position is in visible square:
			add player to playergrid's player list
	loop over gold positions in mastergrid:
		if gold position is in visible square:
			add gold to playergrid's gold list

#### `isVisible`

	loop over rows between player position and checking position:
		if a square is blocking view:
			return false
	loop over columns between player position and checking position:
		if a square is blocking view:
			return false
	return true

#### `gridPrint`

For i < # of players, i++
  Obtain position of player (x, y)
  If (x,y) = currentPostion
    Grid[y][x] = “@”
  Else
    Grid[y][x] = “i+65” (ascii conversion in character form) 
  For i < # of gold piles, i++
    Obtain position of pile, (x, y)
    Grid[y][x] = “*”
  For i < slots in grid array
    Print the string in a new line


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

The data structures for the player are the player’s name, which is a char* pointer, its location, which is represented with the _position_ struct, the amount of gold the player has, which is an integer, the rooms it has seen before, which is represented by the _grid_ struct, and whether the player is currently talking to the server, which is a boolean. 

```c
char* name; 
position_t* position; 
int numGold; 
grid_t* grid;
bool isTalking; 
```

### Definition of function prototypes

A function to create a new player struct. It initializes the players location to a random room spot and sets the player's grid to be the empty grid. The server will then call updateGrid on the player's grid and master grid. 

```c
player_t* player_new(char* name, char* letter, position_t* pos); 
```

A function to update the players `position` struct to register a player’s move. This function need not update their grid, for this is handled by the server. This function need not check that the position coordinates are valid since this is also handled by the server. 

```c
void player_move(player_t* player, position_t* newPosition); 
```

A function to delete a player and free all of its memory. 

```c
void player_delete(player_t* player); 
``` 

A function to get the `position` of a player, i.e. its (x, y) coordinates. 

```c
position_t* player_getPosition(player_t* player); 
``` 

A function to increment the number of gold a player has by _numGold_ amount. This function is called by the server when a player steps on a gold pile. 

```c
void player_addGold(player_t* player, int numGold);
```

A function that returns whether or not a player is currently talking to the server. 

```c
bool player_isTalking(player_t* player); 
```

A function to return the grid object of a player.

```c 
grid_t* player_getGrid(player_t* player); 
```

A function to change whether or not the player is talking to the server. 

```c 
void player_changeStatus(player_t* player); 
```
 
### Detailed pseudo code

#### `player_new`

```
if given name, letter, and grid is valid
  generate a random room spot on the grid 
  allocate memory for a new player
  if memory for player can be created: 
    set this spot as the players position
    set the grid for the player to the empty grid 
    initialize other player information
	  return the new player object
if any errors
	return null 
```

#### `player_move` 

```
replace the position of this player with the given position
```

#### `player_delete` 

``` 
if the given player struct is not null
	free the memory for the position and grid of this player
	free the memory for the player object
```

#### `player_getPosition`

```
if player is not null 
	return the position of this player 
else
	return null
```

#### `player_addGold` 

```
if player is not null 
	add the given amount to this player’s numGold count
```

#### `player_isTalking`
```
if player is not null 
	return this player’s bool struct isTalking
else 
	return false 
```

#### `player_getGrid`

```
if player is not null 
	return this player’s grid struct 
else 
	return null
```

#### `player_changeStatus`

```
if player is not null 
  if isTakling is false, make it true 
  if isTalking is true, make it false 
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

A function to delete a player and free all of its memory. 

```c
void spectator_delete(spectator_t* spectator); 
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

---

## Testing plan

### unit testing

We will conduct unit tests according to detailing in the Design Spec, [DESIGN.md](https://github.com/cs50winter2022/nuggets-team-tux/blob/submit-implementation/DESIGN.md). We will individually test each module as we code to ensure each individual unit of the program runs smoothly. 

### integration testing

We will test the server and the client programs extensively, including: 
End game behavior 
More than MaxPlayers players trying to connect
More than one spectator trying to connect
Testing invalid server address on the client side 

Finally, we will run `client` and `server` on a Thayer Linux server by giving the server's hostname on the client's command line. We’ll run a server on `babylon5` and a client on `plank`. 

We will test both ‘client’ and ‘server’ using the four programs in the shared directory `~/cs50-dev/shared/nuggets/’.` We will also use the `padmap` program, a tool to pad all lines of a mapfile so they have the same length, and the `checkmap` program, which validates a map.

### system testing


We will test the client and server together, looking to check:
The server can take connections
The client can connect to the server
The server can hold up to 26 clients and 1 spectator
The server remembers which player a client was, and fills them in as needed

---

## Limitations

N/A at the moment. 


