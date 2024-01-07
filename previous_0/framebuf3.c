#include "framebuf.h"

#include <dos.h>
#include <stddef.h>
#include <string.h>

/*
486/66 result:

no VSYNC:
6648
6703
6703
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

void make_compact_asm3()
{
    asm volatile(
        // dsti*4
        "xor %%edi, %%edi\n"

        "_loop:\n"
        // stupid at&t: disp(base, index, scale)
        "mov %0+0*4(,%%edi,4), %%ebx\n"
        "mov %0+1*4(,%%edi,4), %%ecx\n"
        "mov %0+2*4(,%%edi,4), %%esi\n"
        "mov %0+3*4(,%%edi,4), %%edx\n"

        // mix!
        "mov %%esi, %%eax\n"
        "mov %%dl, %%ah\n"
        "shl $16, %%eax\n"
        "mov %%bl, %%al\n"
        "mov %%cl, %%ah\n"
        // store packed0
        "mov %%eax, %1+0*320*240/4(%%edi)\n"

        // shift
        "shr $8, %%esi\n"

        // mix!
        "mov %%esi, %%eax\n"
        "mov %%dh, %%ah\n"
        "shl $16, %%eax\n"
        "mov %%bh, %%al\n"
        "mov %%ch, %%ah\n"

        // store packed1
        "mov %%eax, %1+1*320*240/4(%%edi)\n"

        // shift
        "shr $8, %%esi\n"
        "shr $16, %%edx\n"
        "shr $16, %%ebx\n"
        "shr $16, %%ecx\n"

        // mix!
        "mov %%esi, %%eax\n"
        "mov %%dl, %%ah\n"
        "shl $16, %%eax\n"
        "mov %%bl, %%al\n"
        "mov %%cl, %%ah\n"

        // store packed2
        "mov %%eax, %1+2*320*240/4(%%edi)\n"

        // shift
        "shr $8, %%esi\n"

        // mix!
        "mov %%esi, %%eax\n"
        "mov %%dh, %%ah\n"
        "shl $16, %%eax\n"
        "mov %%bh, %%al\n"
        "mov %%ch, %%ah\n"

        // store packed3
        "mov %%eax, %1+3*320*240/4(%%edi)\n"

        "add $4, %%edi\n"
        "cmp $320*240/4, %%edi\n"
        "jb _loop\n"
        :
        : "m"(framebuf), "m"(packed)
        : "cc", "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi");
}

void framebuf_flip(uint8_t* VGA)
{
    make_compact_asm3();

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
