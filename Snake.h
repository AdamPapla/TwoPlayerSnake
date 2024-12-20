#ifndef SNAKE_H_
#define SNAKE_H_
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <deque>
#include <random>
#include <unordered_set>
#include "raylib.h"
#include "Utility.h"

using namespace std::chrono_literals;
using Clock = std::chrono::high_resolution_clock;

class Block {
 public: 
   Block() = default;
   Block( int xInit, int yInit, int bSize ) : 
       x { xInit }, y { yInit }, blockSize { bSize } {

   }
   bool operator==( const Block & rhs ) const {
      return rhs.x == x && rhs.y == y;
   }
   int x;
   int y;
 private:
   int blockSize;
};

// Define hash function for Block
namespace std {
    template <>
    struct hash<Block> {
        std::size_t operator()(const Block& block) const {
            // Combine x and y to create a hash value
            return std::hash<int>{}(block.x) ^ (std::hash<int>{}(block.y) << 1);
        }
    };
}

class SnakeGameLogic {
 public: 
   SnakeGameLogic( int bSize, int blocks ) : blockSize { bSize },
                                    gridBlocks { blocks } {
      snake.push_front( Block( 4, 4, bSize ) );

      // Seed the rng
      std::random_device rd;
      rng = std::mt19937( rd() ); // Standard mersenne_twister_engine seeded with rd()
      distrib = std::uniform_int_distribution<> ( 0, gridBlocks - 1 );

      spawnFood();
   }

   bool update( Move move );
   Block makeNextBlock( Move move );


   const std::unordered_set< Block > & getBlocks() {
      return occupied;
   }
   const Block & getFood() {
      return food;
   }

 private:
   void spawnFood();

   int blockSize;
   int gridBlocks;
   std::uniform_int_distribution<> distrib;
   std::mt19937 rng; // Standard mersenne_twister_engine seeded with rd()
   std::deque< Block > snake;
   std::unordered_set< Block > occupied;
   Block food;
};

class SnakeGame {
 public:
   SnakeGame( int wSize, int grid ) : winSize { wSize }, 
                                      gridBlocks { grid },
                                      alive { true },
                                      highScore { 0 } {
      int maxGridSize = static_cast< int >( winSize * 0.9 );
      blockSize = maxGridSize / gridBlocks;
      gridSize = blockSize * gridBlocks;
      borderWidth = static_cast< int >( ( winSize - gridSize ) / 2 );

      snake = std::make_unique< SnakeGameLogic >( blockSize, gridBlocks );
      InitWindow( winSize, winSize, "Initial basic window text" );
      SetTargetFPS( 60 );
   }
   void run() {
      Move currentDir = Move::up;
      std::chrono::time_point< Clock > frameStart = Clock::now();
      auto frameEnd = Clock::now();

      while ( !WindowShouldClose() ) {
         BeginDrawing();
         ClearBackground( RAYWHITE );
         makeGrid();
         score = snake->getBlocks().size();
         DrawText( "SNAKE", 
                   0.4 * winSize,
                   0.01 * winSize,
                   0.05 * winSize, 
                   DARKGRAY );
         DrawText( TextFormat( "Score:%i", score ), 
                   0.85 * winSize,
                   0.02 * winSize,
                   0.03 * winSize, 
                   DARKGRAY );
         if ( alive ) {
            currentDir = handleInput( currentDir );
            if ( frameEnd - frameStart > 200ms ) {
               frameStart = frameEnd;
               alive = snake->update( currentDir );
            }
            printBlocks( snake->getBlocks() );
            printBlock( snake->getFood() );
            frameEnd = Clock::now();
         } else {
            deathScreen();
         }
         EndDrawing();
      }
   }

   void deathScreen() {
      DrawRectangle( borderWidth + 0.25 * gridSize, borderWidth + 0.25 * gridSize, gridSize/2, gridSize/2, LIGHTGRAY );
      DrawText( "YOU DIED :(", 
                borderWidth + 0.35 * gridSize, 
                borderWidth + 0.45 * gridSize, 
                0.05 * gridSize, 
                DARKGRAY );
      highScore = ( score > highScore ) ? score : highScore;
      DrawText( TextFormat( "Your Score: %i\tHigh Score: %i", score, highScore ), 
                borderWidth + 0.3 * gridSize, 
                borderWidth + 0.55 * gridSize, 
                0.03 * gridSize, 
                DARKGRAY );
      DrawText( "Press <spacebar> to restart\nPress <esc> to exit",
                borderWidth + 0.3 * gridSize, 
                borderWidth + 0.65 * gridSize, 
                0.02 * gridSize, 
                DARKGRAY );
      if ( IsKeyDown( KEY_SPACE ) ) {
         snake = std::make_unique< SnakeGameLogic >( blockSize, gridBlocks );
         alive = true;
      } else if ( IsKeyDown( KEY_ESCAPE ) ) {
         exit( 1 );
      }
   }
   ~SnakeGame() {
      CloseWindow();
   }
 private:
   void makeGrid() {
      // Draw horizontal lines
      for ( int i = 0; i <= gridBlocks; ++i ) {
         const int constOffset = borderWidth + blockSize * i;
         const int start = borderWidth;
         const int end = start + gridSize;
         // Vertical lines
         DrawLine( start, constOffset, end, constOffset, LIGHTGRAY );
         // Horizontal lines
         DrawLine( constOffset, start, constOffset, end, LIGHTGRAY );
      }
   }
   void printBlocks( const std::unordered_set< Block > & blocks ) {
      for ( const Block & block : blocks ) {
         printBlock( block );
      }
   }

   void printBlock( const Block & block ) {
      int xOffset = borderWidth + block.x * blockSize;
      int yOffset = borderWidth + block.y * blockSize;
      DrawRectangle( xOffset, yOffset, blockSize - 1, blockSize - 1, DARKGRAY );
   }

   Move handleInput( Move direction ) {
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

   bool alive;
   int score;
   int highScore;
   const int winSize;
   int gridSize;
   const int gridBlocks;
   int blockSize;
   int borderWidth;
   std::unique_ptr< SnakeGameLogic > snake;
};
#endif
