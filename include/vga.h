/* Paint for DOS - VGA Graphics Driver
 * Copyright (C) 2024
 */

#ifndef _VGA_H_
#define _VGA_H_

#include "types.h"

/* VGA Mode 13h - 320x200 256 colors */
#define VGA_WIDTH  320
#define VGA_HEIGHT 200
#define VGA_MEMORY 0xA0000

/* Standard VGA colors */
#define COLOR_BLACK   0
#define COLOR_BLUE    1
#define COLOR_GREEN   2
#define COLOR_CYAN    3
#define COLOR_RED     4
#define COLOR_MAGENTA 5
#define COLOR_BROWN   6
#define COLOR_WHITE   7
#define COLOR_GRAY    8
#define COLOR_YELLOW  14
#define COLOR_BRIGHT_WHITE 15

/* VGA Functions */
void vga_init(void);
void vga_cleanup(void);
void vga_putpixel(int x, int y, u8 color);
u8 vga_getpixel(int x, int y);
void vga_clear(u8 color);
void vga_line(int x1, int y1, int x2, int y2, u8 color);
void vga_rect(int x1, int y1, int x2, int y2, u8 color, int filled);
void vga_circle(int cx, int cy, int radius, u8 color, int filled);
void vga_triangle(int x1, int y1, int x2, int y2, int x3, int y3, u8 color, int filled);

#endif /* _VGA_H_ */
