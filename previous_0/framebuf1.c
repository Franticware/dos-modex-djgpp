#include "framebuf.h"

#include <dos.h>
#include <stddef.h>

/*
486/66 result:

no VSYNC:
8626
8626
8681
*/

const char* framebuf_str = __FILE__;

uint8_t framebuf[320 * 240];

#define SC_INDEX 0x03c4 /* VGA sequence controller */
#define SC_DATA 0x03c5
#define CRTC_INDEX 0x03d4 /* VGA CRT controller */

#define MAP_MASK 0x02 /* Sequence controller registers */

#define SR 0x3da /* Input Status Register */
#define VRETRACE 0x08

inline void memcpy5(uint8_t* dstb, uint8_t* src, int count)
{
    uint32_t* dst = (uint32_t*)dstb;
    int srci = 0;
    for (int dsti = 0; dsti != count; ++dsti)
    {
        uint8_t a = src[srci];
        srci += 4;
        uint8_t b = src[srci];
        srci += 4;
        uint8_t c = src[srci];
        srci += 4;
        uint8_t d = src[srci];
        srci += 4;

        dst[dsti] = (a) | ((b) << 8) | ((c) << 16) | ((d) << 24);
    }
}

void framebuf_flip(uint8_t* VGA)
{
    static int odd = 0;
    if (odd)
    {
        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 1);
        memcpy5(VGA, framebuf, 4800);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 2);
        memcpy5(VGA, framebuf + 1, 4800);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 4);
        memcpy5(VGA, framebuf + 2, 4800);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 8);
        memcpy5(VGA, framebuf + 3, 4800);

#ifdef VSYNC_ON
        while ((inportb(SR) & VRETRACE))
            ;
#endif

        outpw(CRTC_INDEX, 0x000c);
        outpw(CRTC_INDEX, 0x000d);

#ifdef VSYNC_ON
        while (!(inportb(SR) & VRETRACE))
            ;
#endif
    }
    else
    {
        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 1);
        memcpy5(VGA + 19200, framebuf, 4800);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 2);
        memcpy5(VGA + 19200, framebuf + 1, 4800);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 4);
        memcpy5(VGA + 19200, framebuf + 2, 4800);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 8);
        memcpy5(VGA + 19200, framebuf + 3, 4800);

#ifdef VSYNC_ON
        while ((inportb(SR) & VRETRACE))
            ;
#endif

        outpw(CRTC_INDEX, 0x4b0c);
        outpw(CRTC_INDEX, 0x000d);

#ifdef VSYNC_ON
        while (!(inportb(SR) & VRETRACE))
            ;
#endif
    }
    odd = !odd;
}
