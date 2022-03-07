# Nuggets Testing Plan 
## Team Tux 
## Georgia Mandell, Daniel Dong, Emily Gao, Jefferson Adjetey

integration testing = client alone or server alone), system testing = client + server together.

### Integration Testing

#### Server

The server code was tested with the client in the shared folder. We started the server and used the announced port number to call to the shared client program, and testing adding players and spectators, picking up gold, and quitting players. We testing having one player join, seeing if the server could receive messages and appropriately respond by adding the player to the list of players, generating their map, and sending their GRID and DISPLAY message. We then testing moving the player around and picking up gold, verifying receipt of correct GOLD and DISPLAY messages based on what their new location in the grid is / if they stepped on gold. We then added a spectator, and testing whether the spectators DISPLAY and GOLD messages were being appropriately updated with the player's movements. 

We also testing multiple players joining, and then quitting. We testing players quitting from the game in a few different cases. We tested a player quitting with no players remaining, in which case it would quit, and we tested a player quitting with players still remaining, confirming that the quitted player was removed from remaining players' display. We also testing if a spectator joins and then players join and quit, in which case the game ends and the spectator sees the game over summary. If a specator joins and then quits without any players joining, we tested that the game still allows more players. But if players have already joined and they all quit, the game quits. 

We tested that the server would accept up to 26 players, and deny a player joining if there are already 26 players. 

We also tested collecting all of the gold, and confirming that when all of the gold is collected, game ends and every client receieves game over summary.

We tested erroneous cases, such as an invalid pathname to a map file, a non-integer seed, and a negative seed. We also tested incorrect input from the client (invalid keystrokes).


### System Testing

We did not test the client and the system together, since our client functionality was not complete. 

Thank you to Professor Palmer for providing the shared code of the client that we were able to test our server on. 
