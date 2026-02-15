/* Paint for DOS - Input Handler (Keyboard and Mouse)
 * Copyright (C) 2024
 */

#ifndef _INPUT_H_
#define _INPUT_H_

#include "types.h"

/* Key codes */
#define KEY_ESC    0x01
#define KEY_1      0x02
#define KEY_2      0x03
#define KEY_U      0x16
#define KEY_L      0x26
#define KEY_S      0x1F
#define KEY_C      0x2E
#define KEY_R      0x13
#define KEY_T      0x14
#define KEY_K      0x25
#define KEY_W      0x11
#define KEY_G      0x22
#define KEY_B      0x30
#define KEY_RED    0x13  /* R */
#define KEY_Y      0x15  /* Y for yellow */

/* Mouse buttons */
#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_RIGHT_BUTTON  0x02
#define MOUSE_MIDDLE_BUTTON 0x04

/* Input functions */
void input_init(void);
void input_cleanup(void);
int input_kbhit(void);
u8 input_getkey(void);
int input_mouse_init(void);
void input_mouse_show(void);
void input_mouse_hide(void);
void input_mouse_get_pos(int *x, int *y, int *buttons);

#endif /* _INPUT_H_ */
