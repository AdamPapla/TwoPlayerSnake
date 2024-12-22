# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Project name and language
project(SnakeGame LANGUAGES CXX)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wshadow -Wconversion -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# Define library with game files
ADD_LIBRARY( SnakeLibModule
   SnakeGame.cpp Utility.cpp SnakeGameLogic.cpp
)

# Find and link raylib
target_link_libraries( SnakeLibModule -lraylib )

# Set the path to the raylib headers (assuming Raylib headers are in /usr/local/include)
include_directories(/usr/local/include)

ADD_EXECUTABLE( SnakeGame main.cpp )

target_link_libraries( SnakeGame SnakeLibModule )

# Custom clean target (optional, mirrors Makefile clean behavior)
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Clean up build artifacts."
)

