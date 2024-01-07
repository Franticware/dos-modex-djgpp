#include "framebuf.h"

#include <dos.h>
#include <stddef.h>
#include <string.h>

/*
486/66 result:
10109
10164

no VSYNC:
7637
7637
7692
*/

const char* framebuf_str = __FILE__;

uint8_t framebuf[320 * 240];

#define SC_INDEX 0x03c4 /* VGA sequence controller */
#define SC_DATA 0x03c5
#define CRTC_INDEX 0x03d4 /* VGA CRT controller */

#define MAP_MASK 0x02 /* Sequence controller registers */

#define SR 0x3da /* Input Status Register */
#define VRETRACE 0x08

uint32_t packed[4][320 * 240 / 4 / 4];

inline uint32_t mix_lo(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return ((uint8_t)a) | (((uint8_t)b) << 8) | (((uint8_t)c) << 16) | (((uint8_t)d) << 24);
}

void make_compact()
{
    uint32_t* colors = (uint32_t*)framebuf;
    for (int dsti = 0; dsti != 320 * 240 / 4 / 4; ++dsti)
    {
        int i = dsti << 2;

        // x86 addressing: should boil down to dsti*4+offset
        uint32_t a = colors[i + 0];
        uint32_t b = colors[i + 1];
        uint32_t c = colors[i + 2];
        uint32_t d = colors[i + 3];

        packed[0][dsti] = mix_lo(a, b, c, d);

        a >>= 8;
        b >>= 8;
        c >>= 8;
        d >>= 8;

        packed[1][dsti] = mix_lo(a, b, c, d);

        a >>= 8;
        b >>= 8;
        c >>= 8;
        d >>= 8;

        packed[2][dsti] = mix_lo(a, b, c, d);

        a >>= 8;
        b >>= 8;
        c >>= 8;
        d >>= 8;

        packed[3][dsti] = mix_lo(a, b, c, d);
    }
}

void framebuf_flip(uint8_t* VGA)
{
    make_compact();

    static int odd = 0;
    if (odd)
    {
        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 1);
        memcpy(VGA, packed[0], 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 2);
        memcpy(VGA, packed[1], 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 4);
        memcpy(VGA, packed[2], 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 8);
        memcpy(VGA, packed[3], 19200);

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
        memcpy(VGA + 19200, packed[0], 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 2);
        memcpy(VGA + 19200, packed[1], 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 4);
        memcpy(VGA + 19200, packed[2], 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 8);
        memcpy(VGA + 19200, packed[3], 19200);

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
