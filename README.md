# cli-maze
Command line based maze rendering for Linux
This software is inspired by CommandLineFPS:
https://github.com/OneLoneCoder/CommandLineFPS
or https://www.youtube.com/watch?v=xW8skO7MFYw

The goal is to propese pretty much identical software although instead using Windows API 
it provides support for Linux by implementing terminal controls over ncurses library.

# Install
1. Install ncurses lib.
   * sudo apt-get install libncurses5-dev libncursesw5-dev
3. CMake configuration:
* mkdir build && cd build/
* cmake ..
* make

# Running
1. Set the terminal to xterm.
export TERM=xterm
2. For good impression set your terminal deafult geometry to 120x40.

