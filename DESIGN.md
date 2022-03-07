# CS50 Nuggets
## Design Spec
### Team Tux, Winter 2022

According to the [Requirements Spec] (REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes the player, spectator and grid modules. 
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Client

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


The data structures for the client is the game struct and the score struct.

The game struct has the following parameters: 
  * char hostname[messageMaxLength];   // the hostname
  * char port[maxPortLength];      // the port
  * addr_t serverAddress;      // the server's address
  * char playerName[messageMaxLength]; // the player's name
  * int cX;    //the x cordinate of the cursor
  * int cY;    //the y cordinate of the cursor


The score struct has the following parameters:
  * char letter;   //players letter
  * int collected; //nuggets collected 
  * int nuggetsLeft; //nuggets left to be collected
  * int purse;   //player's gold purse

----

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

Input (map file): The server receives a pathname for a map file from the command-line. The server may assume that the map file is valid; it just needs to verify that the file can be opened for reading. 

Output (terminal): The server only prints the “game-over summary” when the number of nuggets reaches 0. See more on the content of the game-over summary in the Requirements Spec. 

The message module used in the server also logs useful information and messages into a log file, including: 
  - Hostname port
  - All players name
  - Each time player picks up gold, and how much
  - Remaining gold
  - After quit, all players gold amount
  - Any connection errors 

### Functional decomposition into modules

We anticipate the following functions and modules: 
  1. _main_
  2. _parseArgs_, parse command-line arguments and initialize seed 
  3. _initializeGame_, initialize game struct
  4. _handleMessages_, handler function for _message_loop_
  5. _handlePlayMessage_, message handler for `PLAY` messages which calls grid and sender functions
  6. _handleSpectateMessage_, message handler for `SPECTATE` message which calls grid and sender functions
  7. _handleKeyMessage_, message handler for `KEY` message which calls grid and sender functions
  8. _updateAllGrids_, update every players grid 
  9. _sendDisplayToAll_, which sends `DISPLAY` messages to all clients
  10. _sendGoldToAll_, sends `GOLD` messages to all clients
  11. _sendQuitMessage_, _sendOkMessage_, _sendGridMessage_, _sendGoldMessage_, _sendErrorMessage_, _sendDisplayMessage_, _sendSpecDisplayMessage_ are sender functions that construct messages and call _message_send_ 

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


### Major data structures

The server has one major data structures: the _game_ struct, which is a single global variable accessible to both the client and server. The _game_ struct keeps track of a list of player struct in _game.players_, the master grid struct in _game.masterGrid_, and the spectator struct with _game.spectator_.


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
