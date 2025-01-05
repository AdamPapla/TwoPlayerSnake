#include "SnakeGameLogic.h"

void
SnakeGameLogic::update( Move move, Snake & snake, U32 playerNum ) {
   Block newBlock = makeNextBlock( move, snake );
   updateTail( newBlock, snake );
   if ( occupied.contains( newBlock ) ) {
      snake.alive = false;
   }
   snake.body.push_front( newBlock );
   occupied.insert( { newBlock, playerNum } );
}

void
SnakeGameLogic::updateTail( Block newHead, Snake & snake ) {
   if ( newHead != food && snake.body.size() != 0 ) {
      occupied.erase( snake.body.back() );
      snake.body.pop_back();
   } else {
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
