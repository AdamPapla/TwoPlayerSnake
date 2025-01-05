#include <format>
#include "SnakeGame.h"

void SnakeGame::run() {
   currentDirP1 = Move::up;
   currentDirP2 = Move::down;
   std::chrono::time_point< Clock > frameStart = Clock::now();
   auto frameEnd = Clock::now();
   bool scoreUpdated = false;

   while ( !WindowShouldClose() ) {
      BeginDrawing();
      ClearBackground( RAYWHITE );
      makeGrid();
      header();
      if ( !isGameStarted ) {
         startScreen();
      } else if ( !winner.has_value() ) {
         scoreUpdated = false;
         handleInput();
         if ( frameEnd - frameStart > 250ms ) {
            frameStart = frameEnd;
            winner = snakeLogic->update( currentDirP1, currentDirP2 );
         }
         printBlocks( snakeLogic->getBlocks() );
         printBlock( snakeLogic->getFood(), DARKGRAY );
         frameEnd = Clock::now();
      } else {
         if ( !scoreUpdated ) {
            updateScore();
            scoreUpdated = true;
         }
         deathScreen();
      }
      EndDrawing();
   }
}

void SnakeGame::header() {
   DrawText( "SNAKE", 
             offsetPosition( 0.45 ),
             scaledSize( 0.01 ),
             scaledSize( 0.05 ),
             DARKGRAY );
   if ( snakeLogic ) {
      std::string score1;
      std::string score2;
      if ( numPlayers == 1 )  {
         score1 = std::format( "Score:{}", snakeLogic->lenSnake1() );
         score2 = std::format( "High Score:{}", highScore );
      } else {
         score1 = std::format( "Player 1: {}", snakeLogic->lenSnake1() );
         score2 = std::format( "Player 2: {}", snakeLogic->lenSnake2() );
      }
      DrawText( score1.c_str(),
                offsetPosition( 0.05 ),
                scaledSize( 0.02 ),
                scaledSize( 0.03 ),
                DARKGRAY );
      DrawText( score2.c_str(),
                offsetPosition( 0.85 ),
                scaledSize( 0.02 ),
                scaledSize( 0.03 ),
                DARKGRAY );
   }
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
   std::string scoreMsg;
   if ( numPlayers == 1 ){
      winnerMsg = "You died :(";
      scoreMsg = std::format( "Your Score: {}\nHigh Score: {}", scoreP1, highScore );
   } else if ( winner.value() == 0 ) {
      winnerMsg = "You both lose :(";
      scoreMsg = std::format( "Player One Score: {}\nPlayer Two Score: {}", scoreP1, scoreP2 );
   } else {
      winnerMsg = std::format( "Player {} wins", winner.value() );
      scoreMsg = std::format( "Player One Score: {}\nPlayer Two Score: {}", scoreP1, scoreP2 );
   }
   DrawRectangle( offsetPosition( 0.25 ),
                  offsetPosition( 0.25 ),
                  scaledSize( 0.5 ), 
                  scaledSize( 0.5 ), 
                  LIGHTGRAY );
   DrawText( winnerMsg.c_str(), 
             offsetPosition( 0.3 ),
             offsetPosition( 0.45 ),
             scaledSize( 0.05 ),
             DARKGRAY );
   DrawText( scoreMsg.c_str(), 
             offsetPosition( 0.3 ),
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
