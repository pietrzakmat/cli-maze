# cli-maze
Command line based maze rendering for Linux
This software is inspired by CommandLineFPS:
https://github.com/OneLoneCoder/CommandLineFPS
or https://www.youtube.com/watch?v=xW8skO7MFYw

The goal is to propese pretty much identical software although instead using Windows API 
it provides support for Linux by implementing terminal controls over ncurses library.

# Install
1. Install ncurses lib.
2. CMake configuration:
 2.1 mkdir build && cd build/
 2.2 cmake..
 2.3 make

# Running
1. Set the terminal to xterm.
export TERM=xterm
2. For good impression set your terminal deafult geometry to 120x40.

