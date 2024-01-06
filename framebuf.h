#ifndef FRAMEBUF_H
#define FRAMEBUF_H

#include <stdint.h>

// #define VSYNC_ON

extern const char* framebuf_str;

extern uint8_t framebuf[320 * 240];

void framebuf_flip(uint8_t* VGA);

#endif // FRAMEBUF_H
