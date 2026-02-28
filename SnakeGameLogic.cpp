#include "SnakeGameLogic.h"

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
   if ( occupied.contains(snake1.head()) ) {
      snake1.alive = false;
   }
   if ( occupied.contains(snake2.head()) ) {
      snake2.alive = false;
   }
   // Now that we've checked if the snake has died, update the new head positions
   occupied.emplace( snake1.head(), 1 );
   occupied.emplace( snake2.head(), 2 );
   if ( foodRequired ) {
      spawnFood();
   }
}

void
SnakeGameLogic::spawnFood() {
   Block newFood ( distrib( rng ), distrib( rng ), blockSize );
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
   Block newBlock ( xOld, yOld, blockSize );
   switch( move ) {
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
