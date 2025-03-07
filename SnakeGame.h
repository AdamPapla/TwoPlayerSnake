#ifndef SNAKEGAME_H_
#define SNAKEGAME_H_
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <deque>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include "raylib.h"
#include "Utility.h"
#include "SnakeGameLogic.h"

using namespace std::chrono_literals;
using Clock = std::chrono::high_resolution_clock;
class SnakeGame {
 public:
   SnakeGame( U32 wSize, U32 grid ) : gridBlocks { grid },
                                      isGameStarted { false },
                                      scoreP1 { 0 },
                                      scoreP2 { 0 },
                                      highScore { 0 } {
      U32 maxGridSize = static_cast< U32 >( wSize * 0.9 );
      blockSize = static_cast< U32 >( maxGridSize / gridBlocks );
      gridSize = blockSize * gridBlocks;
      borderWidth = static_cast< U32 >( ( wSize - gridSize ) / 2 );

      InitWindow( wSize, wSize, "Initial basic window text" );
      SetTargetFPS( 60 );
   }
   void run();
   void header();
   void startScreen();
   void deathScreen();

   ~SnakeGame() {
      CloseWindow();
   }
 private:
   void makeGrid();

   void printBlocks( const std::unordered_map< Block, U32 > & blocks ) {
      for ( const auto & [ block, player ] : blocks ) {
         Color color = ( player == 1 ) ? DARKPURPLE : DARKBLUE;
         printBlock( block, color );
      }
   }

   void printBlock( const Block & block, const Color color ) {
      U32 xOffset = blockToPosition( block.x );
      U32 yOffset = blockToPosition( block.y );
      DrawRectangle( xOffset, yOffset, blockSize - 1, blockSize - 1, color );
   }

   Move handleInputWasd( Move direction ) {
      if ( IsKeyDown( KEY_A ) ) {
         return Move::left;
      } else if ( IsKeyDown( KEY_D ) ) {
         return Move::right;
      } else if ( IsKeyDown( KEY_S ) ) {
         return Move::down;
      } else if ( IsKeyDown( KEY_W ) ) {
         return Move::up;
      }
      return direction;
   }

   Move handleInputArrows( Move direction ) {
      if ( IsKeyDown( KEY_LEFT ) ) {
         return Move::left;
      } else if ( IsKeyDown( KEY_RIGHT )) {
         return Move::right;
      } else if ( IsKeyDown( KEY_DOWN )) {
         return Move::down;
      } else if ( IsKeyDown( KEY_UP )) {
         return Move::up;
      }
      return direction;
   }

   void handleInput() {
      currentDirP1 = handleInputWasd( currentDirP1 );
      if ( numPlayers == 2 ) {
         currentDirP2 = handleInputArrows( currentDirP2 );
      }
   }

   void updateScore() {
      if ( numPlayers == 1 ) {
         scoreP1 = snakeLogic->lenSnake1();
         highScore = std::max( scoreP1, highScore );
      } else {
         if ( winner.value() == 1 ) {
            ++scoreP1;
         } else if ( winner.value() == 2 ) {
            ++scoreP2;
         }
      }
   }

   U32 blockToPosition( U32 blockNum ) {
      return borderWidth + blockSize * blockNum;
   }

   U32 offsetPosition( double gridFraction ) {
      return static_cast< U32 >( borderWidth + gridFraction * gridSize );
   }

   U32 scaledSize( double gridFraction ) {
      return static_cast< U32 >( gridFraction * gridSize );
   }

   const U32 gridBlocks;
   bool isGameStarted;

   Move currentDirP1;
   Move currentDirP2;

   std::optional< U32 > winner;
   U32 numPlayers;
   U32 scoreP1;
   U32 scoreP2;
   U32 highScore;
   U32 gridSize;
   U32 blockSize;
   U32 borderWidth;
   std::unique_ptr< SnakeGameLogic > snakeLogic;
};

#endif
