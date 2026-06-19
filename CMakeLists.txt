cmake_minimum_required(VERSION 3.14)
project(SudokuGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ──────────────────────────────────────────────
# Fetch raylib from source (static build)
# ──────────────────────────────────────────────
include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# ──────────────────────────────────────────────
# Game executable
# ──────────────────────────────────────────────
add_executable(SudokuGame
    src/main.cpp
    src/sudoku.cpp
    src/game.cpp
    src/renderer.cpp
)

target_include_directories(SudokuGame PRIVATE src)
target_link_libraries(SudokuGame PRIVATE raylib)

# On Windows: suppress the console window in Release builds
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(SudokuGame PRIVATE -mwindows)
    endif()
endif()
