#include "Utility.h"

std::string getDir( Move move ) {
   switch( move ) {
      case Move::left:
         return "left";
      case Move::right:
         return "right";
      case Move::down:
         return "down";
      case Move::up:
         return "up";
      case Move::stay:
         return "stay";
   }
   return "";
}
