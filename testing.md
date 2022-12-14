# Nuggets Testing Plan 
## Team Tux 
### Georgia Mandell, Daniel Dong, Emily Gao, Jefferson Adjetey

#### Note to grader: Our client functionality is the not complete. Please test our game on the professor's client. We have spoken to the professor about this, and he said it was okay. 

### Integration Testing

#### Server

The server code was tested with the client in the shared folder. We started the server and used the announced port number to call to the shared client program, and testing adding players and spectators, picking up gold, and quitting players. We testing having one player join, seeing if the server could receive messages and appropriately respond by adding the player to the list of players, generating their map, and sending their GRID and DISPLAY message. We then testing moving the player around and picking up gold, verifying receipt of correct GOLD and DISPLAY messages based on what their new location in the grid is / if they stepped on gold. We then added a spectator, and testing whether the spectators DISPLAY and GOLD messages were being appropriately updated with the player's movements. 

We also tested multiple players joining, and then quitting. We tested players quitting from the game in a few different cases. We tested a player quitting with no players remaining in the game but less than 26 players have joined, in which case the game does not quit and waits for more players. We tested a player quitting with players and/or spectators still remaining, confirming that the quitted player was removed from remaining players' / spectator's display. We tested adding 26 players and then quitting all 26 players, in which case the game will end, since no more players can join. 

We also tested if a spectator joins and then players join and quit, in which case the game ends and the spectator sees the game over summary. If a specator joins and then quits without any players joining, we tested that the game still allows more players. If the spectator joins and all 26 players join and then quit, send game over message to spectator. If a new spectator is added when we already have a spectator, we tested that the old spectator is replaced and sent a QUIT message.

We tested that the server would accept up to 26 players, and deny a player joining if there are already 26 players. 

We also tested collecting all of the gold, and confirming that when all of the gold is collected, game ends and every client still talking to server receieves game over summary.

We tested erroneous cases, such as an invalid pathname to a map file, a non-integer seed, and a negative seed. We also tested incorrect input from the client (invalid keystrokes).

### Unit Testing

#### Grid

We tested the functions of grid on gridTester.c before having the server call any of the grid functionalities.

We created new grid's given various maps and various inputs, to see whether or not grid would be able to handle them.

Running every function of grid to ensure that each operated properly on its own.
* creating a new grid
* creating a masterGrid
* creating a grid for a new player
* updating the player's visbility
* printing the user's visibility
* moving the player on the map
* deleting the grid
* We ran ALL functions with valgrind to ensure no memory leaks

After these tests, we integrated grid into server to complete integration testing.

### System Testing

We did not test the client and the system together, since our client functionality was not complete. 

Thank you to Professor Palmer for providing the shared code of the client that we were able to test our server on. 
