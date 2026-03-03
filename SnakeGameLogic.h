#ifndef SNAKEGAMELOGIC_H_
#define SNAKEGAMELOGIC_H_
#include "Utility.h"
#include "raylib.h"
#include <algorithm>
#include <chrono>
#include <deque>
#include <iostream>
#include <optional>
#include <random>
#include <thread>
#include <unordered_map>
#include <unordered_set>

using namespace std::chrono_literals;
using Clock = std::chrono::high_resolution_clock;
using U32 = uint32_t;

class Block {
 public:
   Block() = default;
   Block( U32 xInit, U32 yInit, U32 bSize )
       : x{ xInit }, y{ yInit }, blockSize{ bSize } {}
   bool operator==( const Block & rhs ) const { return rhs.x == x && rhs.y == y; }
   U32 x;
   U32 y;

 private:
   U32 blockSize;
};

// Define hash function for Block
namespace std {
template <>
struct hash< Block > {
   std::size_t operator()( const Block & block ) const {
      // Combine x and y to create a hash value
      return std::hash< U32 >{}( block.x ) ^ ( std::hash< U32 >{}( block.y ) << 1 );
   }
};
} // namespace std

struct Snake {
   Snake() : alive{ true } {}
   const Block head() const { return body.front(); }
   std::deque< Block > body;
   bool alive;
};

class SnakeGameLogic {
 public:
   SnakeGameLogic( U32 bSize, U32 blocks, U32 nPlayers )
       : blockSize{ bSize }, gridBlocks{ blocks }, numPlayers{ nPlayers } {
      snake1.body.push_front( Block( 2, 2, bSize ) );
      if ( numPlayers == 2 ) {
         snake2.body.push_front( Block( gridBlocks - 2, gridBlocks - 2, bSize ) );
      }

      // Seed the rng
      std::random_device rd;
      rng =
          std::mt19937( rd() ); // Standard mersenne_twister_engine seeded with rd()
      distrib = std::uniform_int_distribution<>( 0, gridBlocks - 1 );

      spawnFood();
   }

   std::optional< U32 > update( Move move1, Move move2 ) {
      const Block & oldHead1 = snake1.head();
      const Block & oldHead2 = snake2.head();
      // Update the snakes positions
      updateSnake( move1, snake1, 1 );
      updateSnake( move2, snake2, 2 );
      // Check death conditions and handle spawning food
      updatePostMoves();
      // Handle edge case where one-block snakes' heads pass through each other -
      // death for both players
      if ( snake1.head() == oldHead2 && snake2.head() == oldHead1 ) {
         snake1.alive = false;
         snake2.alive = false;
      }
      // Set winner
      if ( !snake1.alive && !snake2.alive ) {
         winner = 0;
      } else if ( !snake2.alive ) {
         winner = 1;
      } else if ( !snake1.alive ) {
         winner = 2;
      }
      return winner;
   }

   void updateSnake( Move move, Snake & snake, U32 playerNum );
   void updatePostMoves();
   Block makeNextBlock( Move move, const Snake & snake );

   const std::unordered_map< Block, U32 > & getBlocks() { return occupied; }
   const Block & getFood() { return food; }
   U32 lenSnake1() const { return static_cast< U32 >( snake1.body.size() ); }
   U32 lenSnake2() const { return static_cast< U32 >( snake2.body.size() ); }

 private:
   void spawnFood();
   void updateTail( Block newHead, Snake & snake );

   bool foodRequired{ false };

   U32 blockSize;
   U32 gridBlocks;
   U32 numPlayers;

   std::uniform_int_distribution<> distrib;
   std::mt19937 rng; // Standard mersenne_twister_engine seeded with rd()

   Snake snake1;
   Snake snake2;

   std::unordered_map< Block, U32 > occupied;
   std::optional< U32 > winner;
   Block food;
};
#endif
