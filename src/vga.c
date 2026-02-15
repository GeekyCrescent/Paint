/* Paint for DOS - VGA Graphics Driver Implementation
 * Copyright (C) 2024
 */

#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#include "vga.h"

static u8 *vga_mem = (u8 *)VGA_MEMORY;
static int old_mode = 0;

/* Initialize VGA Mode 13h (320x200 256 colors) */
void vga_init(void) {
    union REGS regs;
    
    /* Get current video mode */
    regs.h.ah = 0x0F;
    int86(0x10, &regs, &regs);
    old_mode = regs.h.al;
    
    /* Set VGA Mode 13h */
    regs.h.ah = 0x00;
    regs.h.al = 0x13;
    int86(0x10, &regs, &regs);
}

/* Restore original video mode */
void vga_cleanup(void) {
    union REGS regs;
    
    regs.h.ah = 0x00;
    regs.h.al = old_mode;
    int86(0x10, &regs, &regs);
}

/* Plot a pixel at (x, y) with specified color */
void vga_putpixel(int x, int y, u8 color) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        vga_mem[y * VGA_WIDTH + x] = color;
    }
}

/* Get pixel color at (x, y) */
u8 vga_getpixel(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        return vga_mem[y * VGA_WIDTH + x];
    }
    return 0;
}

/* Clear screen with specified color */
void vga_clear(u8 color) {
    memset(vga_mem, color, VGA_WIDTH * VGA_HEIGHT);
}

/* Draw line using Bresenham's algorithm */
void vga_line(int x1, int y1, int x2, int y2, u8 color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;
    
    while (1) {
        vga_putpixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2)
            break;
            
        e2 = 2 * err;
        
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/* Draw rectangle */
void vga_rect(int x1, int y1, int x2, int y2, u8 color, int filled) {
    int x, y;
    int minx = (x1 < x2) ? x1 : x2;
    int maxx = (x1 > x2) ? x1 : x2;
    int miny = (y1 < y2) ? y1 : y2;
    int maxy = (y1 > y2) ? y1 : y2;
    
    if (filled) {
        for (y = miny; y <= maxy; y++) {
            for (x = minx; x <= maxx; x++) {
                vga_putpixel(x, y, color);
            }
        }
    } else {
        vga_line(minx, miny, maxx, miny, color);  /* top */
        vga_line(maxx, miny, maxx, maxy, color);  /* right */
        vga_line(maxx, maxy, minx, maxy, color);  /* bottom */
        vga_line(minx, maxy, minx, miny, color);  /* left */
    }
}

/* Draw circle using Bresenham's circle algorithm */
void vga_circle(int cx, int cy, int radius, u8 color, int filled) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    int i;
    
    while (x <= y) {
        if (filled) {
            for (i = cx - x; i <= cx + x; i++) {
                vga_putpixel(i, cy + y, color);
                vga_putpixel(i, cy - y, color);
            }
            for (i = cx - y; i <= cx + y; i++) {
                vga_putpixel(i, cy + x, color);
                vga_putpixel(i, cy - x, color);
            }
        } else {
            vga_putpixel(cx + x, cy + y, color);
            vga_putpixel(cx - x, cy + y, color);
            vga_putpixel(cx + x, cy - y, color);
            vga_putpixel(cx - x, cy - y, color);
            vga_putpixel(cx + y, cy + x, color);
            vga_putpixel(cx - y, cy + x, color);
            vga_putpixel(cx + y, cy - x, color);
            vga_putpixel(cx - y, cy - x, color);
        }
        
        x++;
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
    }
}

/* Draw triangle */
void vga_triangle(int x1, int y1, int x2, int y2, int x3, int y3, u8 color, int filled) {
    if (filled) {
        int minx = (x1 < x2) ? ((x1 < x3) ? x1 : x3) : ((x2 < x3) ? x2 : x3);
        int maxx = (x1 > x2) ? ((x1 > x3) ? x1 : x3) : ((x2 > x3) ? x2 : x3);
        int miny = (y1 < y2) ? ((y1 < y3) ? y1 : y3) : ((y2 < y3) ? y2 : y3);
        int maxy = (y1 > y2) ? ((y1 > y3) ? y1 : y3) : ((y2 > y3) ? y2 : y3);
        int x, y;
        
        /* Simple scanline fill - check if point is inside triangle */
        for (y = miny; y <= maxy; y++) {
            for (x = minx; x <= maxx; x++) {
                /* Barycentric coordinate test */
                int d1 = (x - x2) * (y1 - y2) - (x1 - x2) * (y - y2);
                int d2 = (x - x3) * (y2 - y3) - (x2 - x3) * (y - y3);
                int d3 = (x - x1) * (y3 - y1) - (x3 - x1) * (y - y1);
                
                int has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
                int has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
                
                if (!(has_neg && has_pos)) {
                    vga_putpixel(x, y, color);
                }
            }
        }
    } else {
        vga_line(x1, y1, x2, y2, color);
        vga_line(x2, y2, x3, y3, color);
        vga_line(x3, y3, x1, y1, color);
    }
}
