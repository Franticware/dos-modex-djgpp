#include "framebuf.h"

#include <dos.h>
#include <stddef.h>

/*
486/66 result:
20164
20219
20219

no VSYNC:
10824
10824
10769
*/

const char* framebuf_str = __FILE__;

uint8_t framebuf[320 * 240];

#define SC_INDEX 0x03c4 /* VGA sequence controller */
#define SC_DATA 0x03c5
#define CRTC_INDEX 0x03d4 /* VGA CRT controller */

#define MAP_MASK 0x02 /* Sequence controller registers */

#define SR 0x3da /* Input Status Register */
#define VRETRACE 0x08

inline void copy_every_4th(void* dst, const void* src, size_t count)
{
    unsigned char* dstb = (unsigned char*)dst;
    const unsigned char* srcb = (const unsigned char*)src;
    size_t j = 0;
    for (size_t i = 0; i != count; ++i, j += 4)
    {
        dstb[i] = srcb[j];
    }
}

void framebuf_flip(uint8_t* VGA)
{
    static int odd = 0;
    if (odd)
    {
        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 1);
        copy_every_4th(VGA, framebuf, 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 2);
        copy_every_4th(VGA, framebuf + 1, 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 4);
        copy_every_4th(VGA, framebuf + 2, 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 8);
        copy_every_4th(VGA, framebuf + 3, 19200);

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
        copy_every_4th(VGA + 19200, framebuf, 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 2);
        copy_every_4th(VGA + 19200, framebuf + 1, 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 4);
        copy_every_4th(VGA + 19200, framebuf + 2, 19200);

        outp(SC_INDEX, MAP_MASK);
        outp(SC_DATA, 8);
        copy_every_4th(VGA + 19200, framebuf + 3, 19200);

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
