cmake_minimum_required(VERSION 3.10)

# set the project name
project(cli_maze)

# add the executable
add_executable(cli_maze cli_maze.cpp)

target_link_libraries(cli_maze ncursesw)
