# CS50 Nuggets
## Team Tux  
## Server

### Assumptions

Assumptions: The server may assume that a given map is valid, but it shall verify that the file can be opened for reading. 

The server shall be prepared to receive these message types: PLAY, SPECTATE, and KEY.

### Limitations

The server can only accept up to 26 players, and one spectator. If a new player wants to join when we already have maximum number of players, they will receive a QUIT message.

The server terminates when all the gold is collected or all the players have quit. 