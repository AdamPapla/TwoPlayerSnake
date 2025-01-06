# Snake 2 #
## Description ##
A twist on the classic Snake game, this version includes the following modifications

- An optional two player mode - kill or be killed as you compete for food and attempt to outmaneuvre your opponent.
- Score  tracking; track your high score in single player or your head to head record in 2 player.
- Periodic boundary conditions for additional flexibility - especially useful for surprising your opponent in two player mode.

## Installation ## 
Snake 2 currently does not provide pre-built binaries. As such, the source code will need to be downloaded and compiled using the accompanying `CMakeLists.txt` file. To successfully compiler, you'll need:

- gcc
- CMake
- Raylib
- GTest

## Gameplay ##
### Singleplayer ###
Simply use the WASD keys to move. Keep in mind that, unlike classic Snake, boundary conditions are periodic so you don't need to avoid them. Do, however, exercise caution as to not bump into your body upon reemerging!

### Multiplayer ###
For two player mode, player 1 (purple) uses the WASD keys to move while player 2 (blue) uses the arrow keys. Death works the same as singleplayer mode - avoid crashing into your opponent (and yourself!) to survive. Outmaneuvre your opponent to ensure they don't do the same. 

Length will provide a competitve advantage in trying to bring your opponent to a grisly end, but does not in itself win you the game.
