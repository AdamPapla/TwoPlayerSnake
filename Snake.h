#pragma once
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <deque>
#include <unordered_set>
#include "raylib.h"

using namespace std::chrono_literals;
using Clock = std::chrono::high_resolution_clock;

class Block {
 public: 
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


enum class Move {
   left,
   right,
   up,
   down,
   stay
};

std::string getDir( Move move ) {
   switch( move ) {
      case Move::left:
         return "left";
      case Move::right:
         return "right";
      case Move::down:
         return "down";
      case Move::up:
         return "up";
      case Move::stay:
         return "stay";
   }
   return "";
}

class Snake {
 public: 
   Snake( int bSize, int blocks ) : blockSize { bSize },
                                    gridBlocks { blocks } {
      snake.push_front( Block( 4, 4, bSize ) );
      snake.push_front( Block( 4, 5, bSize ) );
      snake.push_front( Block( 4, 6, bSize ) );
      snake.push_front( Block( 4, 7, bSize ) );
   }

   bool Update( Move move, bool eats ) {
      int xOld = snake.front().x;
      int yOld = snake.front().y;
      Block newBlock ( xOld, yOld, blockSize );
      switch( move ) {
         case Move::left:
            newBlock.x = ( xOld - 1 + gridBlocks ) % gridBlocks;
            break;
         case Move::right:
            newBlock.x = ( xOld + 1 + gridBlocks ) % gridBlocks;
            break;
         case Move::down:
            newBlock.y = ( yOld + 1 + gridBlocks ) % gridBlocks;
            break;
         case Move::up:
            newBlock.y = ( yOld - 1  + gridBlocks) % gridBlocks;
            break;
         case Move::stay:
            break;
         default:
            break;
      }
      if ( occupied.contains( newBlock ) ) {
         // Snake dies
         return false;
      } else {
         occupied.insert( newBlock );
         snake.push_front( newBlock );
      }
      if ( !eats ) {
         occupied.erase( snake.back() );
         snake.pop_back();
      }
      return true;
   }

   const std::unordered_set< Block > & getBlocks() {
      return occupied;
   }

 private:
   int blockSize;
   int gridBlocks;
   std::deque< Block > snake;
   std::unordered_set< Block > occupied;
};

class SnakeGame {
 public:
   SnakeGame( int wSize, int grid ) : winSize { wSize }, 
                                      gridBlocks { grid } {
      int maxGridSize = static_cast< int >( winSize * 0.9 );
      blockSize = maxGridSize / gridBlocks;
      gridSize = blockSize * gridBlocks;
      borderWidth = static_cast< int >( ( winSize - gridSize ) / 2 );

      snake = std::make_unique< Snake >( blockSize, gridBlocks );
      InitWindow( winSize, winSize, "Initial basic window text" );
      SetTargetFPS( 60 );
   }
   void run() {
      Move currentDir = Move::up;
      std::chrono::time_point< Clock > frameStart = Clock::now();
      auto frameEnd = Clock::now();

      bool alive = true;
      while ( !WindowShouldClose() ) {
         BeginDrawing();
         ClearBackground( RAYWHITE );
         makeGrid();
         if ( alive ) {
            currentDir = handleInput( currentDir );
            if ( frameEnd - frameStart > 200ms ) {
               frameStart = frameEnd;
               alive = snake->Update( currentDir, false );
               printBlocks( snake->getBlocks() );
            } else {
               printBlocks( snake->getBlocks() );
            }
            frameEnd = Clock::now();
         } else {
            DrawRectangle( borderWidth + 0.25 * gridSize, borderWidth + 0.25 * gridSize, gridSize/2, gridSize/2, LIGHTGRAY );
            DrawText( "YOU DIED :(", borderWidth + 0.35 * gridSize, borderWidth + 0.45 * gridSize, 0.05 * gridSize, DARKGRAY );
            DrawText( TextFormat( "Your Score: %i", snake->getBlocks().size() ), borderWidth + 0.35 * gridSize, borderWidth + 0.55 * gridSize, 0.03 * gridSize, DARKGRAY );
         }
         EndDrawing();
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
      for ( Block block : blocks ) {
         int xOffset = borderWidth + block.x * blockSize;
         int yOffset = borderWidth + block.y * blockSize;
         DrawRectangle( xOffset, yOffset, blockSize - 1, blockSize - 1, DARKGRAY );
      }
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

   const int winSize;
   int gridSize;
   const int gridBlocks;
   int blockSize;
   int borderWidth;
   std::unique_ptr< Snake > snake;
};
