
cmake_minimum_required(VERSION 2.8) 
project(cursed CXX)

set(CMAKE_CXX_STANDARD 11)
set(CURSES_NEED_WIDE TRUE)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
set (SOURCES cursed.cpp  minesweeper.cpp)

add_executable(cursed ${SOURCES})
target_link_libraries(cursed ${CURSES_LIBRARIES})
