#ifndef FRAMEBUF_H
#define FRAMEBUF_H

#include <stdint.h>

extern const char* framebuf_str;
extern uint8_t framebuf[320 * 240];
void framebuf_flip(uint8_t* VGA, int vsync);

#endif // FRAMEBUF_H
