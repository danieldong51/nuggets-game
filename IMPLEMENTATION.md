# CS50 Nuggets
## Design Spec
### Team Tux, Winter 2022

According to the [Requirements Spec] (REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes the player, spectator and grid modules. 
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Player

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.
The client’s only interface with the user is going to be on the command-line. It has two arguments:

```
./client hostname port [playername]
```
	

### Inputs and outputs

The Client will specify whether they are a spectator or a player by sending a message to the server through stdin:
  - PLAY real name if client wants to be a player in the game
  - SPECTATE if client wants to be a spectator in the game

Inputs (keystrokes) - see requirements.md spec for keystrokes

Outputs (display)

The output to the client is a display of _NR+1_ rows and _NC_ columns. The top line shall provide game status, which is different for a player and a spectator. Refer to Requirements.md for details on the game status. 
The remaining _NR_ lines present the grid to the client, which represents the _map_. Details on a valid _map_ are in the Requirements Spec. 
If the client is a spectator, the output is the full map: all gridpoints and occupants. 
If the client is a player at a gridpoint (_pr_, _pc_), that player can see the occupants of only those spots that are visible from its current location, and can see the spots and boundaries of all rooms and passages it has seen since the player began playing, but not their occupants. See more on visibility in the Requirements Spec. 

The client also logs important information into a logfile, including: 
  * Hostname port
  * Playername
  * Each time player picks gold, and how much gold, and total accumulated gold
  * Remaining gold


### Functional decomposition into modules

We anticipate the following modules or functions: 
  1. _handleInput_ - sends messages from client to server
  2. _handleMessage_ - receives messages from server, updates client interface

We make use of the following helper modules: 
  1. _log_, a module that provides a way to log important messages into a file.  
  2. _message_, a module that represents messages sent between the server and the client 

 
### Pseudo code for logic/algorithmic flow

The client will run as follows:

	Initialize the message module (without logging)
	Check arguments
	Command line provides address for server
	If player:
		Send PLAY message to server with player’s real name
	Else:
		Send SPECTATE message to server
	Loop, waiting for input or messages; provide callback functions
	Shut down the message module when down

HandleInput will follow the REQUIREMENTS.spec in sending messages to the server.

HandleMessage will follow the REQUIREMENTS.spec in responding to messages from the server.


### Major data structures


The only data structure for the client is the game struct, which is a single global variable accessible to both the client and server. 

The game struct has the following parameters: 
  * MaxNameLength = 50;   // max number of chars in playerName
  * MaxPlayers = 26;      // maximum number of players
  * GoldTotal = 250;      // amount of gold in the game
  * GoldMinNumPiles = 10; // minimum number of gold piles
  * GoldMaxNumPiles = 30; // maximum number of gold piles

---

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

Input (map file): The server receives a pathname for a map file from the command-line. The server may assume that the map file is valid; it just needs to verify that the file can be opened for reading. 

Output (terminal): The server only prints the “game-over summary” when the number of nuggets reaches 0. See more on the content of the game-over summary in the Requirements Spec. 

The server also logs useful information and messages into a log file, including: 
  - Hostname port
  - All players name
  - Each time player picks up gold, and how much
  - Remaining gold
  - After quit, all players gold amount
  - Any connection errors 

### Functional decomposition into modules

We anticipate the following functions and modules: 
  1. handleMessages = receive message from client, update client interface to be sent back
  2. messageSend = sends message to one specific client

We also define helper modules that provide data structures: 
  1. _player_, a module providing a data structure to represent a player in the game
  2. _spectator_, a module providing a data structure to represent a spectator in the game, which contains information of all player’s names, and locations
  3. _grid_, a module that is used by both the client and server that is used to update the user interface for all clients each time a client sends a message.
  4. _message_, a module that represents messages sent between the server and the client 

### Pseudo code for logic/algorithmic flow

The server will run as follows:

	execute from a command line per the requirement spec
	parse the command line, validate parameters
	call initializeGame() to set up data structures
	initialize the 'message' module
	print the port number on which we wait
	call message_loop(), to await clients
	call gameOver() to inform all clients the game has ended
	clean up


#### main

	Validate command line arguments using parseArgs
	Call initializeGame() with map.txt 
	Initialize ‘message module’ 
	Call acceptMessages()
	

#### parseArgs 

	verify that the given pathname is for a file that can be read 
	call checkMap and padMap on map
	return true if able to open file; false on error

#### acceptMessages

	Print the port number
	message_loop()
	Call visibility

#### Visibility 

	Each time a player presses a valid move keystroke
		For each player:
			Calculate if player sees any new part of grid, update player module
			Calculate which players, gold piles are in the player's visibility
			messageSend to client updated playerGrid, visible players, and visible gold piles
		For spectator:
			messageSend to client grid, with list of players

#### initializeGame 

	populate the global variable “game” with the given map.txt file
	while we have not yet dropped GoldMaxNumPiles piles: 
		if we have dropped at least GoldMinNumPiles piles: 
			break 
		generate a random number, x
		move through the char* grid pointer x times or until you reach a valid room spot
		generate a random number, y, to determine the size of the pile 
		drop a pile of size y on this room spot 


#### gameOver

	If state->remainingGold = 0, end the program
	

### Major data structures

The server has three major data structures. The first is the _game_ struct, which is a single global variable accessible to both the client and server. It also has a _grid_ data structure, which is implemented in the grid module. A grid has the char* _grid_ which is the string display of the map, and a list of _pile_ structures, which contain the location of each pile and the amount of gold in the pile.

Finally, the server keeps track of a list of _player_ objects, which contains information about that player’s name, location, the amount of gold the player has, the rooms it has seen before, and whether the player is currently talking to the server. 


## Testing Plan

### Unit Tests

  1. Test the client, making sure it can connect to a server and handle both sending and receiving messages
  2. Test state module to make sure it can store player positions and gold positions
  3. Test player module to make sure it can handle player positions
  4. Test spectator module
  5. Test visibility to make sure it correctly handles items within visibility when given different player locations and grid layouts

### Integration Tests

  1. Test server
  2. Test game
  3. Test clients
  4. Test spectator

### System Tests

Use ~/cs50-dev/shared/nuggets/ to test

### Usability testing

  1. Test with users (group members)
