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
   SnakeGameLogic( U32 bSize, U32 blocks, bool singlePlayer );

   void applySnapshot( const GameState::Snapshot & snapshot );
   void updateSnakes( const std::unordered_map< ClientId, Move > & moves ) {
      for ( const auto & [ id, move ] : moves ) {
         updateSnake( id, move );
      }
      updatePostMoves();
   }

   U32 getScore( ClientId snakeId ) {
      if ( snakes.contains( snakeId ) ) {
         return lenSnake( snakeId );
      } else if ( finalScores.contains( snakeId ) ) {
         return finalScores.at( snakeId );
      }
      return 0;
   }

   const std::unordered_map< Block, U32 > & getBlocks() { return occupied; }
   const std::unordered_set< Block > & getFood() { return foods; }

   std::optional< int > getWinner() {
      if ( snakes.empty() ) {
         return -1; // No winner
      } else if ( snakes.size() == 1 ) {
         return snakes.begin()->first;
      }
      return std::nullopt;
   }

 private:
   Snake & createSnake( const ClientId snakeId, Block block );
   auto deleteSnake( const ClientId snakeId );
   void updateSnake( ClientId snakeId, Move move );
   void updateTail( Snake & snake );
   void updatePostMoves();
   void cleanupDeadSnakes();
   U32 lenSnake( ClientId clientId ) const;

   Block makeNextBlock( Move move, const Snake & snake );

   void spawnFood();

   U32 foodRequired{ 0 };

   U32 blockSize;
   U32 gridBlocks;
   bool singlePlayer;

   std::uniform_int_distribution<> distrib;
   std::mt19937 rng; // Standard mersenne_twister_engine seeded with rd()

   std::unordered_map< ClientId, Snake > snakes;
   std::unordered_set< Block > foods;
   std::unordered_map< ClientId, U32 > finalScores;

   std::unordered_map< Block, ClientId > oldHeads;

   std::unordered_map< Block, U32 > occupied;
   std::optional< U32 > winner;
   Block food;
};
#endif
