/* 
 * MDFourier for DOS - VGA Vsync Support
 * Copyright (C)2024
 *
 * This file is part of MDFourier
 *
 * Provides hardware vsync synchronization using VGA status registers
 */

#ifndef _VGA_H_
#define _VGA_H_

#include "types.h"

/* VGA Status Register ports */
#define VGA_INPUT_STATUS_1_COLOR 0x3DA  /* Color mode (most common) */
#define VGA_INPUT_STATUS_1_MONO  0x3BA  /* Monochrome mode */

/* VGA Input Status Register 1 bits */
#define VGA_STAT_VSYNC       0x08  /* Bit 3: Vertical Retrace */
#define VGA_STAT_DISPLAY_EN  0x01  /* Bit 0: Display Enable (0 = retrace) */

/* Initialize VGA vsync support */
void vga_init(void);

/* Wait for vertical retrace (vsync) */
void vga_wait_vsync(void);

/* Get current VGA port (color or mono) */
u16 vga_get_status_port(void);

#endif /* _VGA_H_ */
