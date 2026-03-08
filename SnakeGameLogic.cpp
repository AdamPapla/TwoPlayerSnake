#include <cassert>
#include <exception>

#include "SnakeGameLogic.h"

SnakeGameLogic::SnakeGameLogic( U32 bSize, U32 blocks, bool singlePlyr )
    : blockSize{ bSize }, gridBlocks{ blocks }, singlePlayer{ singlePlyr } {
   createSnake( 1, Block{ 2, 2, blockSize } );
   if ( !singlePlayer ) {
      createSnake( 2, Block{ gridBlocks - 2, gridBlocks - 2, bSize } );
   }

   // Seed the rng
   std::random_device rd;
   rng = std::mt19937( rd() ); // Standard mersenne_twister_engine seeded with rd()
   distrib = std::uniform_int_distribution<>( 0, gridBlocks - 1 );

   spawnFood();
}

Snake &
SnakeGameLogic::createSnake( const ClientId snakeId, Block block ) {
   auto [ snakeIt, inserted ] = snakes.emplace( snakeId, Snake{} );
   if ( !inserted ) {
      throw std::runtime_error(
          std::format( "Failed to create snake id {}", snakeId ) );
   }
   Snake & snake = snakeIt->second;
   snake.body.push_front( block );
   occupied.emplace( std::move( block ), snakeId );
   return snake;
}

auto
SnakeGameLogic::deleteSnake( const ClientId snakeId ) {
   auto it = snakes.find( snakeId );
   if ( it == snakes.end() ) {
      throw std::runtime_error( std::format(
          "Attempting to remove non-existent snake with id {}", snakeId ) );
   }
   it = snakes.erase( it );
   std::erase_if( occupied,
                  [ & ]( const auto & p ) { return p.second == snakeId; } );
   return it;
}

void
SnakeGameLogic::updateSnake( ClientId snakeId, Move move ) {
   auto it = snakes.find( snakeId );
   if ( it == snakes.end() ) {
      throw std::runtime_error( std::format(
          "Attempting to update non-existent snake with id {}", snakeId ) );
   }
   Snake & snake = it->second;
   assert( !snake.body.empty() );
   oldHeads.emplace( snake.head(), snakeId );
   Block newBlock = makeNextBlock( move, snake );
   snake.body.push_front( std::move( newBlock ) );
   updateTail( snake );
}

void
SnakeGameLogic::updateTail( Snake & snake ) {
   if ( !foods.contains( snake.head() ) && snake.body.size() != 0 ) {
      occupied.erase( snake.body.back() );
      snake.body.pop_back();
   } else {
      foods.erase( snake.head() );
      occupied.erase( snake.head() );
      ++foodRequired;
   }
}

void
SnakeGameLogic::updatePostMoves() {
   // Note at this point we have updated everything except markign the new
   // head positions as occupied. Ensure the head is moving to a legal place
   // and insert it, else mark the snake as dead
   for ( auto & [ id, snake ] : snakes ) {
      if ( !snake.alive ) {
         // Snake has already been kiled as a result of a head collision
         continue;
      }
      // First, detect head swaps, which should mean death for both snakes
      auto oldHeadIt = oldHeads.find( snake.head() );
      if ( oldHeadIt != oldHeads.end() ) {
         auto & [ oldHead, otherSnakeId ] = *oldHeadIt;
         assert( snakes.contains( otherSnakeId ) );
         Snake & otherSnake = snakes.at( otherSnakeId );
         oldHeadIt = oldHeads.find( otherSnake.head() );
         if ( oldHeadIt != oldHeads.end() && oldHeadIt->second == id ) {
            snake.alive = false;
            otherSnake.alive = false;
         }
      }
      // Now detect head-on-head collisions (both heads end up in same block)
      auto [ it, blockFree ] = occupied.try_emplace( snake.head(), id );
      if ( !blockFree ) {
         snake.alive = false;
         // Check if we have died by colliding with another snake's head
         auto & secondSnake = snakes.at( it->second );
         if ( it->first == secondSnake.head() ) {
            secondSnake.alive = false;
         }
      }
   }
   while ( foodRequired > 0 ) {
      spawnFood();
      foodRequired--;
   }
   cleanupDeadSnakes();
   oldHeads.clear();
}

void
SnakeGameLogic::cleanupDeadSnakes() {
   for ( auto it = snakes.begin(); it != snakes.end(); ) {
      auto & [ id, snake ] = *it;
      if ( !snake.alive ) {
         finalScores[ id ] = static_cast< U32 >( snake.body.size() );
         it = deleteSnake( id );
      } else {
         ++it;
      }
   }
}

U32
SnakeGameLogic::lenSnake( ClientId clientId ) const {
   const auto it = snakes.find( clientId );
   if ( it == snakes.end() ) {
      return 0;
   }
   const Snake & snake = it->second;
   return static_cast< U32 >( snake.body.size() );
}

void
SnakeGameLogic::applySnapshot( const GameState::Snapshot & snapshot ) {
   for ( const auto & snake : snapshot.snakes ) {
      auto [ it, inserted ] = snakes.try_emplace( snake.id, Snake() );
      auto & localSnake = it->second;
      if ( !inserted ) {
         localSnake.body.clear();
      }
      for ( const auto & serdesBlock : snake.blocks ) {
         Block localBlock = Block{ serdesBlock[ 0 ], serdesBlock[ 1 ], blockSize };
         occupied[ localBlock ] = snake.id;
         localSnake.body.push_back( std::move( localBlock ) );
      }
   }
   foods.clear();
   for ( const auto & serdesFood : snapshot.food ) {
      Block foodBlock{ serdesFood[ 0 ], serdesFood[ 1 ], blockSize };
      foods.insert( foodBlock );
   }
}

void
SnakeGameLogic::spawnFood() {
   // TODO: We should add a mechanism here in case we can't find a free block
   Block newFood( distrib( rng ), distrib( rng ), blockSize );
   while ( occupied.contains( newFood ) ) {
      newFood = Block( distrib( rng ), distrib( rng ), blockSize );
   }
   foods.insert( newFood );
}

Block
SnakeGameLogic::makeNextBlock( Move move, const Snake & snake ) {
   U32 xOld = snake.body.front().x;
   U32 yOld = snake.body.front().y;
   Block newBlock( xOld, yOld, blockSize );
   switch ( move ) {
   // Add gridBlocks to ensure we don't take modulo of negative number
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
      newBlock.y = ( yOld - 1 + gridBlocks ) % gridBlocks;
      break;
   case Move::stay:
      break;
   default:
      break;
   }
   return newBlock;
}
