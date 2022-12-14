# Nuggets
## Team Tux 
## Georgia Mandell, Daniel Dong, Emily Gao, Jefferson Adjetey

This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected or all 26 players have joined and quit, the game ends and a summary is printed.

Deconstruction of game: 
* server: a program that handles receiving and messages from client
* grid: a program that implements game logic of moving around grid / picking up gold 
* player: a module to store information about player clients
* spectator: a module to store information about spectator client

Files and directories: 
* server/server.c 
* grid/grid.c, grid/grid.h
* player/player.c, player/player.h
* spectator/spectator.c, spectator/spectator.h
* DESIGN.md - design spec 
* IMPLEMENTATION.md - implementation spec 
* maps/ - playable maps 
* Makefile - makefile 
* Testing.md - testing plan

## Server

The server directory contains server.c, the program that implements all of the game logic.

## Player 

The player directory contains player.c and player.h, which implements the `player` module used in the server. 

## Spectator 

The spectator directory contains spectator.c and spectator.h, which implements the `spectator` module user in the server.

## Client 
The client directory contains client.c, the program that connexts to the server and allows a human to join the game as either player or spectator. Our client functionality is not fully developed. 

## Division of Labor 
- Emily and Dan: grid 
- Georgia: server, player  
- Jeff: spectator, client 

## Materials provided

See the [support library](support/README.md) for some useful modules.

See the [maps](maps/README.md) for some draft maps.
