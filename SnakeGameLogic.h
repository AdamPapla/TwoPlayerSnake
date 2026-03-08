#ifndef SNAKEGAMELOGIC_H_
#define SNAKEGAMELOGIC_H_
#include "Utility.h"
#include <chrono>
#include <deque>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include "GameState.h"

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
   SnakeGameLogic( U32 bSize, U32 blocks, U32 nPlayers );

   std::optional< U32 > update( Move move1, Move move2 );

   void applySnapshot( const GameState::Snapshot & snapshot );

   const std::unordered_map< Block, U32 > & getBlocks() { return occupied; }
   const Block & getFood() { return food; }
   U32 lenSnake1() const { return static_cast< U32 >( snake1.body.size() ); }
   U32 lenSnake2() const { return static_cast< U32 >( snake2.body.size() ); }

 private:
   void updateSnake( Move move, Snake & snake, U32 playerNum );
   void updateTail( Block newHead, Snake & snake );
   void updatePostMoves();

   Block makeNextBlock( Move move, const Snake & snake );

   void spawnFood();

   bool foodRequired{ false };

   U32 blockSize;
   U32 gridBlocks;
   U32 numPlayers;

   std::uniform_int_distribution<> distrib;
   std::mt19937 rng; // Standard mersenne_twister_engine seeded with rd()

   Snake snake1;
   Snake snake2;

   std::unordered_map< ClientId, Snake > snakes;
   std::unordered_set< Block > foods;

   std::unordered_map< Block, U32 > occupied;
   std::optional< U32 > winner;
   Block food;
};
#endif
