#include "Snake.h"

bool
SnakeGameLogic::update( Move move ) {
   Block newBlock = makeNextBlock( move );
   if ( occupied.contains( newBlock ) ) {
      // Snake dies
      return false;
   } else {
      occupied.insert( newBlock );
      snake.push_front( newBlock );
   }
   if ( newBlock != food ) {
      occupied.erase( snake.back() );
      snake.pop_back();
   } else {
      spawnFood();
   }
   return true;
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
SnakeGameLogic::makeNextBlock( Move move ) {
   int xOld = snake.front().x;
   int yOld = snake.front().y;
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
