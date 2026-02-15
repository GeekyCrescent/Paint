/* 
 * MDFourier for DOS - VGA Vsync Support Implementation
 * Copyright (C)2024
 *
 * This file is part of MDFourier
 *
 * Provides hardware vsync synchronization using VGA status registers
 * Based on VGA programming documentation
 */

#include <dos.h>
#include <conio.h>
#include "vga.h"

static u16 vga_status_port = VGA_INPUT_STATUS_1_COLOR;

/* Initialize VGA vsync support - detect color vs mono mode */
void vga_init(void) {
    union REGS regs;
    
    /* Get current video mode via BIOS interrupt 0x10, function 0x0F */
    regs.h.ah = 0x0F;
    int86(0x10, &regs, &regs);
    
    /* Check if monochrome mode (mode 7) */
    if (regs.h.al == 0x07) {
        vga_status_port = VGA_INPUT_STATUS_1_MONO;
    } else {
        vga_status_port = VGA_INPUT_STATUS_1_COLOR;
    }
}

/* Get current VGA status port */
u16 vga_get_status_port(void) {
    return vga_status_port;
}

/* Wait for vertical retrace (vsync) 
 * 
 * Algorithm:
 * 1. Wait for any current retrace to finish (bit goes low)
 * 2. Wait for next retrace to start (bit goes high)
 * 
 * This ensures we wait for a complete frame and don't return
 * immediately if we're called during an existing retrace.
 */
void vga_wait_vsync(void) {
    /* Wait for any current vertical retrace to end */
    while (inp(vga_status_port) & VGA_STAT_VSYNC) {
        /* Busy wait while in retrace */
    }
    
    /* Wait for next vertical retrace to begin */
    while (!(inp(vga_status_port) & VGA_STAT_VSYNC)) {
        /* Busy wait until retrace starts */
    }
    
    /* Now we're at the start of vertical blanking interval (vsync) */
}
