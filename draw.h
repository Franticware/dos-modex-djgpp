#ifndef DRAW_H
#define DRAW_H

#include "framebuf.h"

#define mul320(value) (((value) << 8) + ((value) << 6))

static inline void drawPoint(int32_t x, int32_t y, uint8_t c)
{
    if (x < 0 || x >= 320)
        return;
    if (y < 0 || y >= 240)
        return;
    framebuf[x + mul320(y)] = c;
}

static inline void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t c)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    for (;;)
    {
        // drawPoint(x0,y0,c); // orig
        if (x0 == x1 && y0 == y1)
            break;
        drawPoint(x0, y0, c); // do not draw end point
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

#endif // DRAW_H
