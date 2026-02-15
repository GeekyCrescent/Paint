/* Paint for DOS - Main Application
 * Copyright (C) 2024
 * 
 * A simple paint program for DOS with Adlib/SoundBlaster support
 * Inspired by the Python turtle graphics Paint program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include "types.h"
#include "vga.h"
#include "input.h"
#include "adlib.h"

/* Drawing tools */
typedef enum {
    TOOL_LINE,
    TOOL_SQUARE,
    TOOL_RECTANGLE,
    TOOL_CIRCLE,
    TOOL_TRIANGLE
} Tool;

/* Application state */
typedef struct {
    Tool current_tool;
    u8 current_color;
    int start_x, start_y;
    int has_start;
    int mouse_x, mouse_y;
    int running;
} AppState;

static AppState state;

/* Initialize application state */
void init_state(void) {
    state.current_tool = TOOL_LINE;
    state.current_color = COLOR_WHITE;
    state.start_x = 0;
    state.start_y = 0;
    state.has_start = 0;
    state.mouse_x = VGA_WIDTH / 2;
    state.mouse_y = VGA_HEIGHT / 2;
    state.running = 1;
}

/* Draw UI elements */
void draw_ui(void) {
    char buf[64];
    int i, x, y;
    
    /* Color palette at bottom */
    int colors[] = {COLOR_BLACK, COLOR_WHITE, COLOR_GREEN, COLOR_BLUE, 
                    COLOR_RED, COLOR_YELLOW, COLOR_GRAY, COLOR_CYAN};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    
    for (i = 0; i < num_colors; i++) {
        x = 10 + i * 20;
        y = VGA_HEIGHT - 20;
        vga_rect(x, y, x + 15, y + 15, colors[i], 1);
        
        /* Highlight current color */
        if (colors[i] == state.current_color) {
            vga_rect(x - 2, y - 2, x + 17, y + 17, COLOR_BRIGHT_WHITE, 0);
        }
    }
    
    /* Simple cursor */
    if (state.mouse_x >= 0 && state.mouse_x < VGA_WIDTH && 
        state.mouse_y >= 0 && state.mouse_y < VGA_HEIGHT - 30) {
        vga_line(state.mouse_x - 3, state.mouse_y, state.mouse_x + 3, state.mouse_y, COLOR_BRIGHT_WHITE);
        vga_line(state.mouse_x, state.mouse_y - 3, state.mouse_x, state.mouse_y + 3, COLOR_BRIGHT_WHITE);
    }
}

/* Handle keyboard input */
void handle_keyboard(void) {
    u8 key;
    
    if (!input_kbhit()) {
        return;
    }
    
    key = input_getkey();
    
    switch (key) {
        case KEY_ESC:
            state.running = 0;
            break;
            
        case KEY_U:  /* Undo - clear screen */
            vga_clear(COLOR_BLACK);
            adlib_play_sound(SFX_CLEAR);
            break;
            
        case KEY_L:
            state.current_tool = TOOL_LINE;
            adlib_play_sound(SFX_SELECT);
            break;
            
        case KEY_S:
            state.current_tool = TOOL_SQUARE;
            adlib_play_sound(SFX_SELECT);
            break;
            
        case KEY_C:
            state.current_tool = TOOL_CIRCLE;
            adlib_play_sound(SFX_SELECT);
            break;
            
        case KEY_R:
            state.current_tool = TOOL_RECTANGLE;
            adlib_play_sound(SFX_SELECT);
            break;
            
        case KEY_T:
            state.current_tool = TOOL_TRIANGLE;
            adlib_play_sound(SFX_SELECT);
            break;
            
        case KEY_K:
            state.current_color = COLOR_BLACK;
            break;
            
        case KEY_W:
            state.current_color = COLOR_WHITE;
            break;
            
        case KEY_G:
            state.current_color = COLOR_GREEN;
            break;
            
        case KEY_B:
            state.current_color = COLOR_BLUE;
            break;
            
        case KEY_RED:
            state.current_color = COLOR_RED;
            break;
            
        case KEY_Y:
            state.current_color = COLOR_YELLOW;
            break;
    }
}

/* Handle mouse input */
void handle_mouse(void) {
    int mx, my, buttons;
    static int last_buttons = 0;
    
    input_mouse_get_pos(&mx, &my, &buttons);
    
    state.mouse_x = mx;
    state.mouse_y = my;
    
    /* Check if left button was just pressed */
    if ((buttons & MOUSE_LEFT_BUTTON) && !(last_buttons & MOUSE_LEFT_BUTTON)) {
        /* Ignore clicks in UI area */
        if (my >= VGA_HEIGHT - 30) {
            last_buttons = buttons;
            return;
        }
        
        if (!state.has_start) {
            /* Store start position */
            state.start_x = mx;
            state.start_y = my;
            state.has_start = 1;
        } else {
            /* Draw shape */
            int dx, dy, radius;
            
            switch (state.current_tool) {
                case TOOL_LINE:
                    vga_line(state.start_x, state.start_y, mx, my, state.current_color);
                    break;
                    
                case TOOL_SQUARE:
                    dx = mx - state.start_x;
                    dy = my - state.start_y;
                    if (abs(dx) > abs(dy)) dx = dy;
                    vga_rect(state.start_x, state.start_y, 
                            state.start_x + dx, state.start_y + dx, 
                            state.current_color, 1);
                    break;
                    
                case TOOL_RECTANGLE:
                    vga_rect(state.start_x, state.start_y, mx, my, state.current_color, 1);
                    break;
                    
                case TOOL_CIRCLE:
                    dx = mx - state.start_x;
                    dy = my - state.start_y;
                    radius = (int)sqrt(dx * dx + dy * dy);
                    vga_circle(state.start_x, state.start_y, radius, state.current_color, 1);
                    break;
                    
                case TOOL_TRIANGLE:
                    dx = mx - state.start_x;
                    dy = my - state.start_y;
                    vga_triangle(state.start_x, state.start_y,
                                mx, my,
                                state.start_x - dx, state.start_y + dy,
                                state.current_color, 1);
                    break;
            }
            
            adlib_play_sound(SFX_DRAW);
            state.has_start = 0;
        }
    }
    
    last_buttons = buttons;
}

/* Main function */
int main(void) {
    printf("Paint for DOS v1.0\n");
    printf("Initializing...\n");
    
    /* Initialize subsystems */
    input_init();
    adlib_init();
    
    if (!input_mouse_init()) {
        printf("Warning: Mouse not detected. Using keyboard only mode.\n");
    }
    
    /* Initialize graphics */
    vga_init();
    vga_clear(COLOR_BLACK);
    
    init_state();
    
    printf("\nControls:\n");
    printf("  L - Line tool\n");
    printf("  S - Square tool\n");
    printf("  R - Rectangle tool\n");
    printf("  C - Circle tool\n");
    printf("  T - Triangle tool\n");
    printf("  U - Clear (Undo)\n");
    printf("  K,W,G,B,R,Y - Colors\n");
    printf("  ESC - Exit\n");
    
    /* Main loop */
    while (state.running) {
        handle_keyboard();
        handle_mouse();
        draw_ui();
        
        /* Small delay to prevent CPU hogging */
        delay(10);
    }
    
    /* Cleanup */
    vga_cleanup();
    adlib_cleanup();
    input_cleanup();
    
    printf("\nThank you for using Paint for DOS!\n");
    
    return 0;
}
