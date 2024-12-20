#pragma once

#include <string>

enum class Move {
   left,
   right,
   up,
   down,
   stay
};

std::string getDir( Move move );
