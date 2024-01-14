#include <conio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/nearptr.h>
#include <time.h>

#include "draw.h"
#include "framebuf.h"
#include "vgamodes.h"

int main(int argc, char** argv)
{
    if (__djgpp_nearptr_enable() == 0)
    {
        printf("Could get access to first 640K of memory.\n");
        return 1;
    }

    uint8_t* VGA = (uint8_t*)0xA0000; /* this points to video memory. */
    VGA += __djgpp_conventional_base;

    // modes other than 320x240 are not currently supported by framebuf_flip
    set_unchained_mode(320, 240, VGA);

    const unsigned char colors[30] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,  4,  3,  2,  1,
                                      0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 10, 11, 12, 13, 14};

    const int circle[120] = {155, 220, 144, 219, 134, 217, 124, 213, 115, 209, 106, 204, 97,  198, 89,  191, 82,  183,
                             76,  174, 71,  165, 67,  156, 63,  146, 61,  136, 60,  125, 60,  115, 61,  104, 63,  94,
                             67,  84,  71,  75,  76,  66,  82,  57,  89,  49,  97,  42,  106, 36,  115, 31,  124, 27,
                             134, 23,  144, 21,  155, 20,  165, 20,  176, 21,  186, 23,  196, 27,  205, 31,  214, 36,
                             223, 42,  231, 49,  238, 57,  244, 66,  249, 75,  253, 84,  257, 94,  259, 104, 260, 115,
                             260, 125, 259, 136, 257, 146, 253, 156, 249, 165, 244, 174, 238, 183, 231, 191, 223, 198,
                             214, 204, 205, 209, 196, 213, 186, 217, 176, 219, 165, 220};

    int colorIndex = 0;

    int x0Index = 0;
    int y0Index = 1;

    int x1Index = 60;
    int y1Index = 61;

    int benchmark = 1;

    // measure the time it takes to render 600 frames
    clock_t startTime = clock();

    for (int i = 0; i != 600; ++i)
    {
        if (kbhit())
        {
            if ((int)(getch()) == 27)
            {
                benchmark = 0;
                break;
            }
        }
        memset(framebuf, colors[colorIndex >> 2] + 16, 320 * 240);

        // if (colorIndex >= 60)
        drawLine(circle[x0Index], circle[y0Index], circle[x1Index], circle[y1Index], 45);

        ++colorIndex;
        if (colorIndex >= 120)
            colorIndex = 0;

        x0Index += 2;
        y0Index += 2;
        x1Index += 2;
        y1Index += 2;

        if (x0Index >= 120)
            x0Index -= 120;
        if (x1Index >= 120)
            x1Index -= 120;
        if (y0Index >= 120)
            y0Index -= 120;
        if (y1Index >= 120)
            y1Index -= 120;

        framebuf_flip(VGA, 0);
    }

    clock_t endTime = clock();

    set_mode(TEXT_MODE);

    if (benchmark)
    {
        int timeDiffMs = endTime - startTime;
        timeDiffMs = timeDiffMs * 1000 / CLOCKS_PER_SEC;
        printf("%s: benchmark took %d ms\n", framebuf_str, timeDiffMs);
    }

    __djgpp_nearptr_disable();

    return 0;
}
