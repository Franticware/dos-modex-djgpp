#ifndef VGAMODES_H
#define VGAMODES_H

#include <dos.h>
#include <stdint.h>

#define VIDEO_INT 0x10          /* the BIOS video interrupt. */
#define SET_MODE 0x00           /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE 0x13 /* use to set 256-color mode. */
#define TEXT_MODE 0x03          /* use to set 80x25 text mode. */

#define MISC_OUTPUT 0x03c2 /* VGA misc. output register */
#define SC_INDEX 0x03c4    /* VGA sequence controller */
#define SC_DATA 0x03c5
#define PALETTE_INDEX 0x03c8 /* VGA digital-to-analog converter */
#define PALETTE_DATA 0x03c9
#define CRTC_INDEX 0x03d4 /* VGA CRT controller */

#define MAP_MASK 0x02 /* Sequence controller registers */
#define MEMORY_MODE 0x04

#define H_TOTAL 0x00 /* CRT controller registers */
#define H_DISPLAY_END 0x01
#define H_BLANK_START 0x02
#define H_BLANK_END 0x03
#define H_RETRACE_START 0x04
#define H_RETRACE_END 0x05
#define V_TOTAL 0x06
#define OVERFLOW 0x07
#define MAX_SCAN_LINE 0x09
#define PAGE_MSB 0x0C
#define PAGE_LSB 0x0D
#define V_RETRACE_START 0x10
#define V_RETRACE_END 0x11
#define V_DISPLAY_END 0x12
#define OFFSET 0x13
#define UNDERLINE_LOCATION 0x14
#define V_BLANK_START 0x15
#define V_BLANK_END 0x16
#define MODE_CONTROL 0x17

#define NUM_COLORS 256 /* number of colors in mode 0x13 */

#define SR 0x3da /* Input Status Register */
#define VRETRACE 0x08

/* write a word to a port */
static inline void word_out(uint16_t port, uint8_t reg, uint8_t value)
{
    outpw(port, (value << 8) | reg);
}

static inline void set_mode(uint8_t mode)
{
    union REGS regs;

    regs.h.ah = SET_MODE;
    regs.h.al = mode;
    int86(VIDEO_INT, &regs, &regs);
}

static inline void set_unchained_mode(int width, int height, uint8_t* VGA)
{
    uint16_t i;
    uint32_t* ptr = (uint32_t*)VGA;

    /* set mode 13 */
    set_mode(VGA_256_COLOR_MODE);

    /* turn off chain-4 mode */
    word_out(SC_INDEX, MEMORY_MODE, 0x06);

    /* set map mask to all 4 planes for screen clearing */
    word_out(SC_INDEX, MAP_MASK, 0xff);

    /* clear all 256K of memory */
    for (i = 0; i < 0x4000; i++)
        *ptr++ = 0;

    /* turn off long mode */
    word_out(CRTC_INDEX, UNDERLINE_LOCATION, 0x00);

    /* turn on byte mode */
    word_out(CRTC_INDEX, MODE_CONTROL, 0xe3);

    if (width == 360)
    {
        /* turn off write protect */
        word_out(CRTC_INDEX, V_RETRACE_END, 0x2c);

        outp(MISC_OUTPUT, 0xe7);
        word_out(CRTC_INDEX, H_TOTAL, 0x6b);
        word_out(CRTC_INDEX, H_DISPLAY_END, 0x59);
        word_out(CRTC_INDEX, H_BLANK_START, 0x5a);
        word_out(CRTC_INDEX, H_BLANK_END, 0x8e);
        word_out(CRTC_INDEX, H_RETRACE_START, 0x5e);
        word_out(CRTC_INDEX, H_RETRACE_END, 0x8a);
        word_out(CRTC_INDEX, OFFSET, 0x2d);

        /* set vertical retrace back to normal */
        word_out(CRTC_INDEX, V_RETRACE_END, 0x8e);
    }
    else
    {
        outp(MISC_OUTPUT, 0xe3);
    }

    if (height == 240 || height == 480)
    {
        /* turn off write protect */
        word_out(CRTC_INDEX, V_RETRACE_END, 0x2c);

        word_out(CRTC_INDEX, V_TOTAL, 0x0d);
        word_out(CRTC_INDEX, OVERFLOW, 0x3e);
        word_out(CRTC_INDEX, V_RETRACE_START, 0xea);
        word_out(CRTC_INDEX, V_RETRACE_END, 0xac);
        word_out(CRTC_INDEX, V_DISPLAY_END, 0xdf);
        word_out(CRTC_INDEX, V_BLANK_START, 0xe7);
        word_out(CRTC_INDEX, V_BLANK_END, 0x06);
    }

    if (height == 400 || height == 480)
    {
        word_out(CRTC_INDEX, MAX_SCAN_LINE, 0x40);
    }

    if (width == 320 && height == 240)
    {
        word_out(CRTC_INDEX, PAGE_LSB, 0x00);
    }
}

#endif // VGAMODES_H
