#pragma once

#include <cstdint>
#include <string>

#define GRID_SIZE 100
using ClientId = std::uint16_t;
using Coord = std::array< std::uint32_t, 2 >;

enum class Move { left, right, up, down, stay };

std::string getDir( Move move );
