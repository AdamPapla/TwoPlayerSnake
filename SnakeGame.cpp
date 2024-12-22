#include <format>
#include "SnakeGame.h"

void SnakeGame::run() {
   Move currentDirP1 = Move::up;
   Move currentDirP2 = Move::down;
   std::chrono::time_point< Clock > frameStart = Clock::now();
   auto frameEnd = Clock::now();

   while ( !WindowShouldClose() ) {
      BeginDrawing();
      ClearBackground( RAYWHITE );
      makeGrid();
      header();
      if ( !isGameStarted ) {
         startScreen();
      } else if ( !winner.has_value() ) {
         currentDirP1 = handleInputWasd( currentDirP1 );
         if ( numPlayers == 2 ) {
            currentDirP2 = handleInputArrows( currentDirP2 );
         } else {
            currentDirP1 = handleInputArrows( currentDirP1 );
         }
         if ( frameEnd - frameStart > 250ms ) {
            frameStart = frameEnd;
            winner = snakeLogic->update( currentDirP1, currentDirP2 );
         }
         printBlocks( snakeLogic->getBlocks() );
         printBlock( snakeLogic->getFood(), DARKGRAY );
         frameEnd = Clock::now();
      } else {
         deathScreen();
      }
      EndDrawing();
   }
}

void SnakeGame::header() {
   score = snakeLogic ? static_cast< U32 >( snakeLogic->getBlocks().size() ) : 0;
   DrawText( "SNAKE", 
             offsetPosition( 0.45 ),
             scaledSize( 0.01 ),
             scaledSize( 0.05 ),
             DARKGRAY );
   DrawText( TextFormat( "Score:%i", score ), 
             offsetPosition( 0.85 ),
             scaledSize( 0.02 ),
             scaledSize( 0.03 ),
             DARKGRAY );

}

void SnakeGame::startScreen() {
   DrawRectangle( offsetPosition( 0.25 ),
                  offsetPosition( 0.25 ),
                  scaledSize( 0.5 ),
                  scaledSize( 0.5 ),
                  LIGHTGRAY );
   DrawText( "Welcome to Snake",
             offsetPosition( 0.35 ),
             offsetPosition( 0.45 ),
             scaledSize( 0.04 ),
             DARKGRAY );
   DrawText( "Press 1 for singleplayer\nPress 2 for two player",
             offsetPosition( 0.35 ),
             offsetPosition( 0.55 ),
             scaledSize( 0.03 ),
             DARKGRAY );
   if ( IsKeyDown( KEY_ONE ) ){
      numPlayers = 1;
      isGameStarted = true;
      snakeLogic = std::make_unique< SnakeGameLogic >( blockSize, gridBlocks, numPlayers );
   } else if ( IsKeyDown( KEY_TWO ) ) {
      numPlayers = 2;
      isGameStarted = true;
      snakeLogic = std::make_unique< SnakeGameLogic >( blockSize, gridBlocks, numPlayers );
   }
}

void SnakeGame::deathScreen() {
   std::string winnerMsg;
   if ( numPlayers == 1 ){
      winnerMsg = "You died :(";
   } else if ( winner.value() == 0 ) {
      winnerMsg = "You both lose :(";
   } else {
      winnerMsg = std::format( "Player {} wins", winner.value() );
   }
   DrawRectangle( offsetPosition( 0.25 ),
                  offsetPosition( 0.25 ),
                  scaledSize( 0.5 ), 
                  scaledSize( 0.5 ), 
                  LIGHTGRAY );
   DrawText( winnerMsg.c_str(), 
             offsetPosition( 0.35 ),
             offsetPosition( 0.45 ),
             scaledSize( 0.05 ),
             DARKGRAY );
   highScore = ( score > highScore ) ? score : highScore;
   DrawText( TextFormat( "Your Score: %i\nHigh Score: %i", score, highScore ), 
             offsetPosition( 0.35 ),
             offsetPosition( 0.55 ),
             scaledSize( 0.03 ),
             DARKGRAY );
   DrawText( "Press <spacebar> to restart\nPress <esc> to exit",
             offsetPosition( 0.3 ),
             offsetPosition( 0.65 ),
             scaledSize( 0.03 ),
             DARKGRAY );
   if ( IsKeyDown( KEY_SPACE ) ) {
      snakeLogic = std::make_unique< SnakeGameLogic >( blockSize, gridBlocks, numPlayers );
      winner = std::nullopt;
   } else if ( IsKeyDown( KEY_ESCAPE ) ) {
      exit( 1 );
   }
}

void SnakeGame::makeGrid() {
   // Draw horizontal lines
   for ( U32 i = 0; i <= gridBlocks; ++i ) {
      const U32 constOffset = borderWidth + blockSize * i;
      const U32 start = borderWidth;
      const U32 end = start + gridSize;
      // Vertical lines
      DrawLine( start, constOffset, end, constOffset, LIGHTGRAY );
      // Horizontal lines
      DrawLine( constOffset, start, constOffset, end, LIGHTGRAY );
   }
}
