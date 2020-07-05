#include <stdlib.h>
#include <math.h>
#include <wchar.h>
#include "ncursesw/ncurses.h"
#include <locale.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stropts.h>
#include <unistd.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>


int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}


int main()
{
    size_t nScreenWidth = 120;			// Console Screen Size X (columns)
    size_t nScreenHeight = 40;			// Console Screen Size Y (rows)

    int nMapWidth = 16;				// World Dimensions
    int nMapHeight = 16;

    float fPlayerX = 8.0f;			// Player Start Position
    float fPlayerY = 8.0f;
    float fPlayerA = 0.0f;			// Player Start Rotation

    float fFOV = static_cast<float>(M_PI_4);
    float fDepth = 16.0f;
//    float fSpeed = 5.0f;			// Walking Speed

    std::unique_ptr<char[]> screen = std::make_unique<char[]>(nScreenWidth*nScreenHeight);

    setlocale(LC_ALL, ""); // set before initscr() !
    /* initialize curses */
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
//    noecho();
//    nodelay(this, true);

    std::string map;
    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..........#...#";
    map += "#..........#...#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#........#######";
    map += "#..............#";
    map += "#..............#";
    map += "################";

    int ch = 1;

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    do {
        clear();

        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float elapsed = elapsedTime.count();

        float fDistanceToWall = 0.0f;
        float fRayAngle = fPlayerA - fFOV/2.0f;
        float fEyeX = sinf(fRayAngle);
        float fEyeY = cosf(fRayAngle);
        int nTestX = static_cast<int>(fPlayerX + fEyeX * fDistanceToWall);
        int nTestY = static_cast<int>(fPlayerY + fEyeY * fDistanceToWall);

        for (size_t x = 0; x < nScreenWidth; ++x)
        {
            fRayAngle = (fPlayerA - fFOV/2.0f) + (static_cast<float>(x)/static_cast<float>(nScreenWidth)) * fFOV;

            bool hitWall = false;
            fDistanceToWall = 0.0f;
            fEyeX = sinf(fRayAngle);
            fEyeY = cosf(fRayAngle);

            while (!hitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;

                nTestX = static_cast<int>(fPlayerX + fEyeX * fDistanceToWall);
                nTestY = static_cast<int>(fPlayerY + fEyeY * fDistanceToWall);

                // test if ray out of bounds
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    hitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    // Ray is inbounds, test if wall block
                    if (map.c_str()[static_cast<size_t>(nTestY * nMapWidth + nTestX)] == '#')
                    {
                        hitWall = true;
                    }
                }
            }

            size_t nCeiling = static_cast<size_t>(static_cast<float>(nScreenHeight / 2.0f) - nScreenHeight / static_cast<float>(fDistanceToWall));
            size_t nFloor = nScreenHeight - nCeiling;

            // wchar_t to encode the Unicode characters
            wchar_t nShade = L' ';
            if (fDistanceToWall <= fDepth / 4.0f)      nShade = L'\u2588'; // Close
            else if (fDistanceToWall <= fDepth / 3.0f) nShade = L'\u2593';
            else if (fDistanceToWall <= fDepth / 2.0f) nShade = L'\u2592';
            else if (fDistanceToWall <= fDepth / 2.0f) nShade = L'\u2591';
            else                                       nShade = L' '; // X // Far away

            for (size_t y=0; y<nScreenHeight; ++y)
            {
                if (y < nCeiling)
                {
                    mvaddch(y, x, ' '); // sky
                }
                else if (y > nCeiling && y <= nFloor) // wall
                {
                    mvaddnwstr(y,x, &nShade, 1);
                }
                else
                {
//                    mvaddch(y, x, ' '); // floor
                    // shade floor based on distance:
                    char shade = ' ';
                    float b = 1.0f - ((static_cast<float>(y) - nScreenHeight/2.0f) / (static_cast<float>(nScreenHeight) / 2.0f));
                    if (b < 0.25f)      shade = '#';
                    else if (b < 0.5f)  shade = 'x';
                    else if (b < 0.75f) shade = '.';
                    else if (b < 0.9f)  shade = '-';
                    else                shade = ' ';
                    mvaddch(y, x, shade); // floor
                }
            }
        }

        char bufLog[100];
        sprintf(bufLog, "X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f distanceToWall=%3.2f",
                static_cast<double>(fPlayerX),
                static_cast<double>(fPlayerY),
                static_cast<double>(fPlayerA),
                1.0f / elapsed,
                static_cast<double>(fDistanceToWall));
        mvaddstr(0, 0, bufLog);

        // Display Map
        for (int nx = 0; nx < nMapWidth; ++nx)
        {
//            for (int ny = 0; ny < nMapWidth; ++ny)
            for (int ny = 0; ny < nMapHeight; ++ny)
            {
                mvaddch(ny+1, nx, map.c_str()[ny * nMapWidth + nx]);
            }
        }
        mvaddch(fPlayerX, fPlayerY, 'P');

//        if ( _kbhit())
        {
            ch = getch();

            const float speed = 1.0f;
            switch (ch) {
            case KEY_UP:
            case 'w':
            case 'W':
                fPlayerX += sinf(fPlayerA) * speed;
                fPlayerY += cosf(fPlayerA) * speed;

                //             Collision detection:
                if (map.c_str()[static_cast<int>(fPlayerY) * nMapWidth + static_cast<int>(fPlayerX)] == '#')
                {
                    // Undo:
                    fPlayerX -= sinf(fPlayerA) * speed;
                    fPlayerY -= cosf(fPlayerA) * speed;
                }

                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                fPlayerX -= sinf(fPlayerA) * speed;
                fPlayerY -= cosf(fPlayerA) * speed;

                // Collision detection:
                if (map.c_str()[static_cast<int>(fPlayerY) * nMapWidth + static_cast<int>(fPlayerX)] == '#')
                {
                    // Undo:
                    fPlayerX += sinf(fPlayerA) * speed;
                    fPlayerY += cosf(fPlayerA) * speed;
                }

                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
                // CCW Rotation (Counter Clock Wise Rotation)
                fPlayerA -= 0.8f;
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                // CW Rotation (Clock Wise Rotation)
                fPlayerA += 0.8f;
                break;
            }
        }

    }
    while ((ch != 'q') && (ch != 'Q'));

    endwin();

    exit(0);
}
