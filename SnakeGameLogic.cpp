#include "SnakeGameLogic.h"

SnakeGameLogic::SnakeGameLogic( U32 bSize, U32 blocks, U32 nPlayers )
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

void
SnakeGameLogic::updateSnake( Move move, Snake & snake, U32 playerNum ) {
   Block newBlock = makeNextBlock( move, snake );
   updateTail( newBlock, snake );
   snake.body.push_front( newBlock );
}

void
SnakeGameLogic::updateTail( Block newHead, Snake & snake ) {
   if ( newHead != food && snake.body.size() != 0 ) {
      occupied.erase( snake.body.back() );
      snake.body.pop_back();
   } else {
      foodRequired = true;
   }
}

void
SnakeGameLogic::updatePostMoves() {
   if ( snake1.head() == snake2.head() ) {
      snake1.alive = false;
      snake2.alive = false;
      return;
   }
   if ( occupied.contains( snake1.head() ) ) {
      snake1.alive = false;
   }
   if ( occupied.contains( snake2.head() ) ) {
      snake2.alive = false;
   }
   // Now that we've checked if the snake has died, update the new head positions
   occupied.emplace( snake1.head(), 1 );
   occupied.emplace( snake2.head(), 2 );
   if ( foodRequired ) {
      spawnFood();
}
}

std::optional< U32 >
SnakeGameLogic::update( Move move1, Move move2 ) {
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

void
SnakeGameLogic::applySnapshot( const GameState::Snapshot & snapshot ) {
   for ( const auto & snake : snapshot.snakes ) {
      auto [ it, inserted ] = snakes.try_emplace( snake.id, Snake() );
      auto & localSnake = it->second;
      if ( !inserted ) {
         localSnake.body.clear();
      }
      for ( const auto & serdesBlock : snake.blocks ) {
         Block localBlock = Block { serdesBlock[ 0 ], serdesBlock[ 1 ], blockSize };
         occupied[ localBlock ] = snake.id;
         localSnake.body.push_back( std::move( localBlock ) );
      }
   }
   foods.clear();
   for ( const auto & serdesFood : snapshot.food ) {
      Block foodBlock { serdesFood[ 0 ], serdesFood[ 1 ], blockSize };
      foods.insert( foodBlock );
   }
}

void
SnakeGameLogic::spawnFood() {
   Block newFood( distrib( rng ), distrib( rng ), blockSize );
   while ( occupied.contains( newFood ) ) {
      newFood = Block( distrib( rng ), distrib( rng ), blockSize );
   }
   food = newFood;
   foodRequired = false;
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
