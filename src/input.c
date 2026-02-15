/* Paint for DOS - Input Handler Implementation
 * Copyright (C) 2024
 */

#include <dos.h>
#include <conio.h>
#include "input.h"

static int mouse_available = 0;

/* Initialize input system */
void input_init(void) {
    /* Nothing special needed for keyboard */
}

/* Cleanup input system */
void input_cleanup(void) {
    /* Nothing special needed */
}

/* Check if a key has been pressed */
int input_kbhit(void) {
    return kbhit();
}

/* Get the scan code of pressed key */
u8 input_getkey(void) {
    union REGS regs;
    
    regs.h.ah = 0x00;
    int86(0x16, &regs, &regs);
    
    return regs.h.ah;  /* Return scan code */
}

/* Initialize mouse driver */
int input_mouse_init(void) {
    union REGS regs;
    
    /* Reset mouse */
    regs.x.ax = 0x0000;
    int86(0x33, &regs, &regs);
    
    mouse_available = (regs.x.ax == 0xFFFF);
    
    if (mouse_available) {
        /* Set mouse range to VGA dimensions */
        regs.x.ax = 0x0007;  /* Set horizontal range */
        regs.x.cx = 0;
        regs.x.dx = VGA_WIDTH - 1;
        int86(0x33, &regs, &regs);
        
        regs.x.ax = 0x0008;  /* Set vertical range */
        regs.x.cx = 0;
        regs.x.dx = VGA_HEIGHT - 1;
        int86(0x33, &regs, &regs);
    }
    
    return mouse_available;
}

/* Show mouse cursor */
void input_mouse_show(void) {
    union REGS regs;
    
    if (mouse_available) {
        regs.x.ax = 0x0001;
        int86(0x33, &regs, &regs);
    }
}

/* Hide mouse cursor */
void input_mouse_hide(void) {
    union REGS regs;
    
    if (mouse_available) {
        regs.x.ax = 0x0002;
        int86(0x33, &regs, &regs);
    }
}

/* Get mouse position and button state */
void input_mouse_get_pos(int *x, int *y, int *buttons) {
    union REGS regs;
    
    if (mouse_available) {
        regs.x.ax = 0x0003;
        int86(0x33, &regs, &regs);
        
        if (x) *x = regs.x.cx;
        if (y) *y = regs.x.dx;
        if (buttons) *buttons = regs.x.bx;
    } else {
        if (x) *x = 0;
        if (y) *y = 0;
        if (buttons) *buttons = 0;
    }
}
